
#include "OsmLayer.h"

#include <stdio.h>

#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QMatrix4x4>
#include <QPainter>
#include <QVector4D>

#include "GLUtil.h"
#include "Util.h"

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

OsmLayer::OsmLayer() : Layer(),
    _tileSource(new OsmTileSource()),
    _worldSize(0.0),
    _tileSize(1.0),
    _currentZoom(0),
    _numEdgeTiles(1),
    _lastResolution(0.),
    _shader(new QOpenGLShaderProgram),
    _labelShader(new QOpenGLShaderProgram),
    _isSetup(false)
{
    connect(_tileSource, SIGNAL(tileReady(OsmIndex)),
                         SLOT(onTileReady(OsmIndex)));
}

OsmLayer::~OsmLayer()
{
    delete _tileSource;
    delete _shader;
    delete _labelShader;
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
    if (!_visible)
        return;
    
    if (pass != Pass_BaseMap)
        return;
    
    if (_lastResolution != viewCtx.getResolution()) {
        _lastResolution = viewCtx.getResolution();
        _currentZoom = _getZoomLevel(viewCtx.getResolution()
                                     * viewCtx.getDevicePixelRatio());
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
    for (unsigned int x = upperLeftX; x <= lowerRightX+1 && x < _numEdgeTiles; x++) {
        for (unsigned int y=upperLeftY; y <= lowerRightY+1 && y < _numEdgeTiles; y++)
        {
            OsmIndex idx(x, y, _currentZoom);
            visibleTiles.insert(idx);
            TileMap::iterator foundTile = _tiles.find(idx);
            if (foundTile == _tiles.end()) {
                TileRefPtr newTile = std::make_shared<Tile>();
                newTile->shader = _shader;
                newTile->index = idx;
                newTile->upperLeft.x = _worldTopLeft.x +
                    (_worldSize * ((double)x / (double)_numEdgeTiles));
                newTile->upperLeft.y = _worldTopLeft.y -
                    (_worldSize * ((double)y / (double)_numEdgeTiles));
                newTile->lowerRight.x = newTile->upperLeft.x + _tileSize;
                newTile->lowerRight.y = newTile->upperLeft.y - _tileSize;
                _tiles.insert(std::pair<OsmIndex, TileRefPtr>(idx, newTile));
                _tileSource->getTile(idx);
                newTile->draw(viewCtx);
            } else {
                foundTile->second->draw(viewCtx);
            }
        }
    }
    
    _drawLabel(viewCtx);
    
    // clean up tiles we no longer display
    for (TileMap::iterator i=_tiles.begin(); i != _tiles.end();) {
        if (visibleTiles.find(i->first) == visibleTiles.end()) {
            _tileSource->cancelTileRequest(i->first);
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
OsmLayer::onTileReady(OsmIndex index)
{
    TileMap::iterator i = _tiles.find(index);
    if (i != _tiles.end()) {
        if (_tileSource->hasFinishedTile(index)) {
            i->second->setImage(_tileSource->retrieveFinishedTile(index));
            emit layerUpdated();
        }
    }
}

OsmLayer::Tile::Tile() :
    shader(NULL)
{
    
}

void
OsmLayer::Tile::draw(const ViewCtx &viewCtx)
{
    if (image.isNull())
        return;
    if (!texture) {
        texture.reset(new QOpenGLTexture(image.convertToFormat(
                                            QImage::Format_RGBA8888),
                                         QOpenGLTexture::DontGenerateMipMaps));
        texture->setMinMagFilters(QOpenGLTexture::Linear,
                                  QOpenGLTexture::Linear);
        texture->setWrapMode(QOpenGLTexture::ClampToEdge);
        image = QImage(); // the GPU has its own copy now
    }
    MapPoint w2c = viewCtx.getWorldToCamera();
    texture->bind(0);
    shader->bind();
    shader->setUniformValue("tex0", 0);
    _drawQuad(viewCtx, w2c + upperLeft, w2c + lowerRight);
    texture->release();
    shader->release();
}

void
OsmLayer::Tile::_drawQuad(const ViewCtx &viewCtx, const MapPoint &upperLeft,
                          const MapPoint &lowerRight)
{
    QMatrix4x4 mvp = viewCtx.getProjection();
    mvp.translate(float(upperLeft.x), float(lowerRight.y), 0.f);
    float width = lowerRight.x - upperLeft.x;
    mvp.scale(width, width, 1.0f);
    shader->setUniformValue("mvp", mvp);
    GLUtil::drawUnitQuad();
}

void
OsmLayer::Tile::setImage(const QImage &newImage)
{
    image = newImage;
    texture.reset();
}

uint
OsmLayer::_getZoomLevel(double resolution) const
{
    uint i;
    for (i = 1; i < numZoomLevels && resolution < _resolutions[i]; i++) {}
    return i;
}

void
OsmLayer::_setup()
{
    Util::loadGLSLShader(QOpenGLShader::Vertex, ":quad.vert", _shader);
    Util::loadGLSLShader(QOpenGLShader::Fragment, ":osmTile.frag", _shader);
    if (!_shader->link())
        qWarning() << "Failed to link OSM tile shader:" << _shader->log();
    
    Util::loadGLSLShader(QOpenGLShader::Vertex, ":quad.vert", _labelShader);
    Util::loadGLSLShader(QOpenGLShader::Fragment, ":label.frag", _labelShader);
    if (!_labelShader->link())
        qWarning() << "Failed to link OSM label shader:" << _labelShader->log();
    
    // Render the copyright label to a texture
    QFont font("Monaco", 80);
    font.setStyleHint(QFont::Monospace);
    QFontMetrics metrics(font);
    QString copyright("(c) OpenStreetMap contributors");
    int width = metrics.horizontalAdvance(copyright);
    QImage image(width, metrics.height(),
                 QImage::Format_RGBA8888_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(0, metrics.ascent(), copyright);
    painter.end();
    _labelTexture.reset(new QOpenGLTexture(image,
                                           QOpenGLTexture::DontGenerateMipMaps));
    _labelTexture->setMinMagFilters(QOpenGLTexture::Linear,
                                    QOpenGLTexture::Linear);
    _labelTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
    // The label's baseline sits ascent() down from the top of the image
    _labelSize = QSizeF(width, metrics.height());
    _labelAscent = metrics.ascent();
    
    _isSetup = true;
}

void
OsmLayer::_drawLabel(const ViewCtx &viewCtx)
{
    // draw the credit label at the lower left
    QOpenGLFunctions_4_1_Core *f = GLUtil::gl();
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // premultiplied
    float res = viewCtx.getResolution();
    float textScale = _labelSize.width() / 10000. * res;
    QMatrix4x4 mvp = viewCtx.getProjection();
    mvp.translate(res*10.0, res*8., 0.f);
    // The text baseline is the origin, and the image extends upward from it
    mvp.translate(0.f, -(_labelSize.height() - _labelAscent) * textScale, 0.f);
    mvp.scale(_labelSize.width() * textScale,
              _labelSize.height() * textScale, 1.0f);
    _labelTexture->bind(0);
    _labelShader->bind();
    _labelShader->setUniformValue("tex0", 0);
    _labelShader->setUniformValue("mvp", mvp);
    _labelShader->setUniformValue("uColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    GLUtil::drawUnitQuad();
    _labelShader->release();
    _labelTexture->release();
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

