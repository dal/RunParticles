#include "TrackLayer.h"

#include "PathUtil.h"
#include "Types.h"

#include "cinder/gl/gl.h"

using namespace cinder;

TrackLayer::TrackLayer(const Track *track) :
_track(track),
_duration(0)
{
    int numPts = track->points.count();
    if (numPts > 0)
        _startTime = QDateTime::fromTime_t(track->points[0].time);
    if (numPts > 1)
        _duration = track->points[numPts-1].time - track->points[0].time;
}

TrackLayer::~TrackLayer() 
{
    // do nothing
}

QString
TrackLayer::name() const
{
    return _track->name;
}

QString
TrackLayer::sport() const
{
    return _track->sport;
}

QDateTime
TrackLayer::startTime() const
{
    return _startTime;
}

/*
 * Linear interpolate between a and b by the fractional f.
 */
double lerp(double a, double b, double f)
{
    return a + f * (b - a);
}

MapPoint lerp(const MapPoint& a, const MapPoint& b, double f)
{
    return MapPoint(lerp(a.x, b.x, f), lerp(a.y, b.y, f));
}

unsigned int
TrackLayer::duration() const
{
    return _duration;
}

void
TrackLayer::project(const ViewCtx* viewCtx)
{
    // Project the track into the hi-res path and compute the bounding box
    int startTime = 0;
    for(int i=0; i < _track->points.count(); i++) {
        TrackPoint pt = _track->points[i];
        if (i == 0)
            startTime = pt.time;
        PathPoint newPt;
        newPt.pos = viewCtx->toProjection(pt.pos);
        newPt.time = pt.time - startTime;
        _path_hi.push_back(newPt);
        _bounds += newPt.pos;
    }
    
    qDebug("hi: %d points", _path_hi.count());
    
    // Make the medium res path
    _path_med = PathUtil::DouglasPeucker(_path_hi, 1.0);
    qDebug("med: %d points", _path_med.count());
    
    // Make the low res path
    _path_lo = PathUtil::DouglasPeucker(_path_med, 0.2);
    qDebug("lo: %d points", _path_lo.count());
}

void
TrackLayer::draw(uint pass, const ViewCtx *viewCtx, const TimeCtx *timeCtx)
{
    if (!_bounds.overlaps(viewCtx->getBoundingBox()))
        return;
        
    switch (pass) {
        case 0:
            _drawPath(viewCtx, timeCtx);
            break;
        case 1:
            _drawParticle(viewCtx);
            break;
    };
}

void
TrackLayer::_drawPath(const ViewCtx *viewCtx, const TimeCtx *timeCtx)
{
    // Choose which path to display
    Path *currentPath = &_path_hi;
    double res = viewCtx->getResolution();
    if (res >= 1.0 && res < 3.0)
        currentPath = &_path_med;
    else if (res >= 3.0)
        currentPath = &_path_lo;
    
    if (_track->sport != "Running")
        gl::color( Color( 0.3, 0.3, 1 ) );
    else
        gl::color( Color( 1, 0, 0 ) );
    
    PathPoint *lastPathPt;
    MapPoint *lastMapPt;
    bool lastInbounds = false;
    for(int i=0; i < currentPath->count(); i++) {
        PathPoint *pt = &(*currentPath)[i];
        MapPoint *thisMapPt = &(pt->pos);
        bool inbounds = viewCtx->getBoundingBox().contains(*thisMapPt);
        if (i == 0 || pt->time < timeCtx->getMapSeconds()) {
            if (i > 0 && (inbounds || lastInbounds)) {
                gl::drawLine( Vec2f(lastMapPt->x, lastMapPt->y),
                             Vec2f(thisMapPt->x, thisMapPt->y));
            }
            lastMapPt = thisMapPt;
            lastPathPt = pt;
            _particlePos = *thisMapPt;
        } else {
            double elapsed = timeCtx->getMapSeconds() -
                             double(lastPathPt->time);
            int trkElapsed = pt->time - lastPathPt->time;
            double f = (trkElapsed == 0) ? 0. : elapsed / double(trkElapsed);
            if (f > 0.0 && (inbounds || lastInbounds)) {
                MapPoint finalPt = lerp(*lastMapPt, *thisMapPt, f);
                gl::drawLine( Vec2f(lastMapPt->x, lastMapPt->y),
                             Vec2f(finalPt.x, finalPt.y) );
                _particlePos = finalPt;
            } else {
                _particlePos = *thisMapPt;
            }
            break;
        }
        lastInbounds = inbounds;
    }
}
            
void
TrackLayer::_drawParticle(const ViewCtx *viewCtx) const
{
    gl::color( Color( 1, 1, 1 ) );
    float radius = viewCtx->getResolution() * 2.0;
    if (radius < 2.0)
        radius = 2.0;
    gl::drawSolidCircle( Vec2f( _particlePos.x, _particlePos.y ), radius);
}

BoundingBox
TrackLayer::getBoundingBox() const
{
    return _bounds;
}

