
#include "OsmLayer.h"

// This is the total number of meters in the world -180. - 180 W,
#define MAXLAT 85.0511287798066
#define MAXLON 180.0

// from http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
int long2tilex(double lon, int z)
{
	return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int lat2tiley(double lat, int z)
{
	return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

double tilex2long(int x, int z)
{
	return x / pow(2.0, z) * 360.0 - 180;
}

double tiley2lat(int y, int z)
{
	double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

void _drawQuad(const MapPoint &upperLeft, float width)
{
    gl::drawSolidRect(Rectf(upperLeft.x, upperLeft.y,
                            upperLeft.x + width, upperLeft.y - width));
}

OsmLayer::OsmLayer() : Layer(),
    _worldSize(0.0),
    _tileSize(1.0),
    _currentZoom(0),
    _numEdgeTiles(1),
    _lastResolution(0.),
    _shader(new QGLShaderProgram),
    _isSetup(false)
{
    
}

OsmLayer::~OsmLayer()
{
    delete _shader;
}

QString
OsmLayer::name() const
{
    return QString("Open Street Map");
}

QString
OsmLayer::sport() const
{
    return QString("map");
}

QDateTime
OsmLayer::startTime() const
{
    return QDateTime();
}

PassMap
OsmLayer::passes() const
{
    PassMap layers;
    layers.insert(Pass_BaseMap);
    return layers;
}

void
OsmLayer::project(const Projection &projection)
{
    _worldTopLeft = projection.toProjection(LonLat(-MAXLON, MAXLAT));
    _worldLowerRight = projection.toProjection(LonLat(MAXLON, -MAXLAT));
    _worldSize = _worldTopLeft.y - _worldLowerRight.y;
    for (int i = 0; i < numZoomLevels; i++)
        _resolutions[i] = (_worldSize / (1 << i)) / pixelsPerTile;
    // a test tile
    double lat = tiley2lat(3165, 13);
    _testTilePos = projection.toProjection(LonLat(tilex2long(1313, 13), lat));
    MapPoint leftTile = projection.toProjection(LonLat(tilex2long(1312, 13), lat));
    _testTileWidth = _testTilePos.x - leftTile.x;
}

void
OsmLayer::draw(uint pass, const ViewCtx &viewCtx, const TimeCtx&)
{
    if (pass != Pass_BaseMap)
        return;
    
    if (_lastResolution != viewCtx.getResolution()) {
        _lastResolution = viewCtx.getResolution();
        _currentZoom = _getZoomLevel(viewCtx.getResolution());
        _numEdgeTiles = pow(2., _currentZoom);
        _tileSize = _worldSize / _numEdgeTiles;
        qDebug("Zoom level %d", _currentZoom);
    }
    
    BoundingBox viewport = viewCtx.getBoundingBox();
    unsigned int upperLeftX, upperLeftY, lowerRightX, lowerRightY;
    _getTileXYAtMapPoint(viewport.upperLeft, &upperLeftX, &upperLeftY);
    _getTileXYAtMapPoint(viewport.lowerRight, &lowerRightX, &lowerRightY);
    
    if (!_isSetup)
        _setup();

    gl::color( Color( 1, 1, 1 ) );
    for (unsigned int x=upperLeftX; x <= lowerRightX; x++) {
        for (unsigned int y=upperLeftY; y <= lowerRightY; y++) {
            double tileUpperLeftX = _worldTopLeft.x + (_worldSize * ((double)x / (double)_numEdgeTiles));
            double tileUpperLeftY = _worldTopLeft.y - (_worldSize * ((double)y / (double)_numEdgeTiles));
            double tileLowerRightX = tileUpperLeftX + _tileSize;
            double tileLowerRightY = tileUpperLeftY - _tileSize;
            gl::drawStrokedRect(RectT<float>(tileUpperLeftX, tileUpperLeftY,
                                             tileLowerRightX, tileLowerRightY));
        }
    }
    
    /*
    _testTexture->enableAndBind();
	_shader->bind();
	_shader->setUniformValue(_shader->uniformLocation("tex0"), 0 );
    _drawQuad(_testTilePos, _testTileWidth);
    _testTexture->unbind();
    _shader->release();
    */
}

BoundingBox
OsmLayer::getBoundingBox() const
{
    return BoundingBox();
}

MapPoint
OsmLayer::position() const
{
    return MapPoint(0., 0.);
}

uint
OsmLayer::_getZoomLevel(double resolution) const
{
    uint i;
    for (i = 0; i < numZoomLevels && resolution < _resolutions[i]; i++) {}
    return i;
}

void
OsmLayer::_setup()
{
    _shader->addShaderFromSourceCode(QGLShader::Fragment,
                                     "#version 110\n\n"
                                     "uniform sampler2D tex0;\n"
                                     "void main()\n"
                                     "{\n"
                                     "gl_FragColor = texture2D( tex0, gl_TexCoord[0].st);\n"
                                     "}\n");
    ImageSourceRef _testImg = cinder::loadImage( "/Users/dal/tmp/13.1313.3165.png");
    _testTexture = new cinder::gl::Texture(_testImg);
    _isSetup = true;
}


void
OsmLayer::_getTileXYAtMapPoint(const MapPoint &pos, uint *x, uint *y) const
{
    
    *x = int(floor(_numEdgeTiles * ((pos.x - _worldTopLeft.x) / _worldSize)));
    *y = int(floor(_numEdgeTiles * (_worldTopLeft.y - pos.y) / _worldSize));
                                    
}
