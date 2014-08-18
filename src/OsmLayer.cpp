
#include "OsmLayer.h"

#include <stdio.h>

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
	return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 /
                                  cos(lat * M_PI/180.0)) / M_PI) /
                       2.0 * pow(2.0, z)));
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

void _drawQuad(const MapPoint &upperLeft, const MapPoint &lowerRight)
{
    gl::drawSolidRect(Rectf(upperLeft.x, upperLeft.y,
                            lowerRight.x, lowerRight.y));
}

OsmLayer::OsmLayer() : Layer(),
    _tileSource(new OsmTileSource()),
    _worldSize(0.0),
    _tileSize(1.0),
    _currentZoom(0),
    _numEdgeTiles(1),
    _lastResolution(0.),
    _shader(new QGLShaderProgram),
    _isSetup(false)
{
    connect(_tileSource, SIGNAL(tileReady(unsigned int, unsigned int, unsigned int)),
            SLOT(onTileReady(unsigned int, unsigned int, unsigned int)));
}

OsmLayer::~OsmLayer()
{
    delete _shader;
}

QString
OsmLayer::name() const
{
    return QString("OpenStreetMap");
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
    }
    
    BoundingBox viewport = viewCtx.getBoundingBox();
    unsigned int upperLeftX, upperLeftY, lowerRightX, lowerRightY;
    _getTileXYAtMapPoint(viewport.upperLeft, &upperLeftX, &upperLeftY);
    _getTileXYAtMapPoint(viewport.lowerRight, &lowerRightX, &lowerRightY);
    
    if (!_isSetup)
        _setup();

    OsmIndexSet visibleTiles;
    for (unsigned int x = upperLeftX; x <= lowerRightX && x < _numEdgeTiles; x++) {
        for (unsigned int y=upperLeftY; y <= lowerRightY && y < _numEdgeTiles; y++)
        {
            OsmIndex idx(x, y, _currentZoom);
            visibleTiles.insert(idx);
            TileMap::iterator foundTile = _tiles.find(idx);
            if (foundTile == _tiles.end()) {
                Tile *newTile = new Tile();
                newTile->shader = _shader;
                newTile->index = idx;
                newTile->upperLeft.x = _worldTopLeft.x +
                    (_worldSize * ((double)x / (double)_numEdgeTiles));
                newTile->upperLeft.y = _worldTopLeft.y -
                    (_worldSize * ((double)y / (double)_numEdgeTiles));
                newTile->lowerRight.x = newTile->upperLeft.x + _tileSize;
                newTile->lowerRight.y = newTile->upperLeft.y - _tileSize;
                _tiles.insert(std::pair<OsmIndex, Tile*>(idx, newTile));
                _tileSource->getTile(idx.x, idx.y, idx.z);
                newTile->draw(viewCtx);
            } else {
                foundTile->second->draw(viewCtx);
            }
        }
    }
    
    // clean up tiles we no longer display
    for (TileMap::iterator i=_tiles.begin(); i != _tiles.end();) {
        if (visibleTiles.find(i->first) == visibleTiles.end()) {
            delete i->second;
            i = _tiles.erase(i);
        } else {
            ++i;
        }
    }
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

void
OsmLayer::onTileReady(unsigned int x, unsigned int y, unsigned int z)
{
    OsmIndex idx = OsmIndex(x, y, z);
    TileMap::iterator i = _tiles.find(idx);
    if (i != _tiles.end()) {
        i->second->setTexture(_tileSource->retrieveFinishedTile(x, y, z));
        emit layerUpdated();
    }
}

OsmLayer::Tile::Tile() : texture(NULL), shader(NULL)
{
    
}

void
OsmLayer::Tile::draw(const ViewCtx &viewCtx)
{
    // debug, draw diagonal lines
    gl::color( Color( 1, 1, 1 ) );
    MapPoint w2c = viewCtx.getWorldToCamera();
    gl::drawLine(w2c + upperLeft, w2c + lowerRight);
    gl::drawLine(w2c + Vec2d(upperLeft.x, lowerRight.y),
                 w2c + Vec2d(lowerRight.x, upperLeft.y));
    // end debug diagonal lines
    if (texture != NULL) {
        texture->enableAndBind();
        shader->bind();
        shader->setUniformValue(shader->uniformLocation("tex0"), 0 );
        _drawQuad(w2c+ upperLeft, w2c + lowerRight);
        texture->unbind();
        shader->release();
    }
}

void
OsmLayer::Tile::setTexture(const Surface8u &surface)
{
    texture = new cinder::gl::Texture(surface);
}

uint
OsmLayer::_getZoomLevel(double resolution) const
{
    uint i;
    for (i = 1; i <= numZoomLevels && resolution < _resolutions[i]; i++) {}
    return i-1;
}

void
OsmLayer::_setup()
{
    _shader->addShaderFromSourceCode(QGLShader::Fragment,
                                     "#version 110\n\n"
                                     "uniform sampler2D tex0;\n"
                                     "void main()\n"
                                     "{\n"
                                     "vec4 texelColor = texture2D(tex0, gl_TexCoord[0].st);\n"
                                     "vec4 scaledColor = texelColor * vec4(0.3, 0.59, 0.11, 1.0);\n"
                                     "float luminance = scaledColor.r + scaledColor.g + scaledColor.b;\n"
                                     "luminance = 1. - luminance;\n"
                                     "gl_FragColor = vec4( luminance, luminance, luminance, texelColor.a);\n"
                                     "}\n");
    _isSetup = true;
}


void
OsmLayer::_getTileXYAtMapPoint(const MapPoint &pos,
                               unsigned int *x,
                               unsigned int *y) const
{
    int px, py;
    px = int(floor(_numEdgeTiles * ((pos.x - _worldTopLeft.x) / _worldSize)));
    py = int(floor(_numEdgeTiles * (_worldTopLeft.y - pos.y) / _worldSize));
    *x = (px < 0) ? 0 : px;
    *y = (py < 0) ? 0 : py;
}

