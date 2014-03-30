
#include "OsmLayer.h"

// This is the total number of meters in the world -180. - 180 W,
#define MAXLAT 85.0511287798066
#define MAXLON 180.0

OsmLayer::OsmLayer() : Layer(),
    _currentZoom(0),
    _lastResolution(0.)
{
    
}

OsmLayer::~OsmLayer()
{
    
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
    if (_lastResolution != viewCtx.getResolution()) {
        _lastResolution = viewCtx.getResolution();
        _currentZoom = _getZoomLevel(viewCtx.getResolution());
        qDebug("Zoom level %d", _currentZoom);
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

uint
OsmLayer::_getZoomLevel(double resolution) const
{
    uint i;
    for (i = 0; i < numZoomLevels && resolution < _resolutions[i]; i++) {}
    return i;
}

