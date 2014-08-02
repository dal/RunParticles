#include "TrackLayer.h"

#include "PathUtil.h"
#include "Projection.h"
#include "Types.h"
#include "ViewCtx.h"

#define PARTICLE_RADIUS 12.0
#define MEDIUM_LOD_RES 15.0
#define LO_LOD_RES 30.0

using namespace cinder;

const Color TrackLayer::RunColor = Color( 1, 0, 0 );
const Color TrackLayer::OtherColor = Color( 0.3, 0.3, 1 );
const Color TrackLayer::SelectedColor = Color( 1, 1, 0 );

TrackLayer::TrackLayer(const Track *track) : Layer(),
_particleRadius(PARTICLE_RADIUS),
_mediumLodRes(MEDIUM_LOD_RES),
_loLodRes(LO_LOD_RES),
_track(track),
_duration(0)
{
    int numPts = track->points.count();
    if (numPts > 0)
        _startTime = QDateTime::fromTime_t(track->points[0].time);
    if (numPts > 1)
        _duration = track->points[numPts-1].time - track->points[0].time;
    _pathBuffer = (float*)malloc(sizeof(float)*4*numPts);
}

TrackLayer::~TrackLayer() 
{
    free(_pathBuffer);
    _pathBuffer = NULL;
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

PassMap
TrackLayer::passes() const
{
    PassMap layers;
    layers.insert(Pass_UnselectedPath);
    layers.insert(Pass_SelectedPath);
    layers.insert(Pass_Particle);
    return layers;
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
TrackLayer::project(const Projection &projection)
{
    // Project the track into the hi-res path and compute the bounding box
    int startTime = 0;
    for(int i=0; i < _track->points.count(); i++) {
        TrackPoint pt = _track->points[i];
        if (i == 0)
            startTime = pt.time;
        PathPoint newPt;
        newPt.pos = projection.toProjection(pt.pos);
        newPt.time = pt.time - startTime;
        _path_hi.push_back(newPt);
        _bounds += newPt.pos;
    }
    
    _particleRadius *= projection.getScaleMultiplier();
    _mediumLodRes *= projection.getScaleMultiplier();
    _loLodRes *= projection.getScaleMultiplier();
    
    qDebug("med: %f, lo: %f", _mediumLodRes, _loLodRes);
    qDebug("hi: %d points", _path_hi.count());
    
    // Make the medium res path
    _path_med = PathUtil::DouglasPeucker(_path_hi, _mediumLodRes);
    qDebug("med: %d points", _path_med.count());
    
    // Make the low res path
    _path_lo = PathUtil::DouglasPeucker(_path_hi, _loLodRes);
    qDebug("lo: %d points", _path_lo.count());
}

void
TrackLayer::draw(uint pass, const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    if (!visible() || !_bounds.overlaps(viewCtx.getBoundingBox()))
        return;
    bool selected = viewCtx.isSelected(id());
    switch (pass) {
        case Pass_UnselectedPath:
            if (!selected)
                _drawPath(viewCtx, timeCtx);
            break;
        case Pass_SelectedPath:
            if (selected)
                _drawPath(viewCtx, timeCtx);
            break;
        case Pass_Particle:
            _drawParticle(viewCtx);
            break;
    };
}

void
TrackLayer::_drawPath(const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    // Choose which path to display
    Path *currentPath = &_path_hi;
    double res = viewCtx.getResolution();
    if (res >= _mediumLodRes && res < _loLodRes)
        currentPath = &_path_med;
    else if (res >= _loLodRes)
        currentPath = &_path_lo;
    
    if (viewCtx.isSelected(id()))
        gl::color( SelectedColor );
    else if (_track->sport == "Running")
        gl::color( RunColor );
    else
        gl::color( OtherColor );
    
    PathPoint *lastPathPt;
    MapPoint *lastMapPt;
    int bufferIndex = 0;
    bool lastInbounds = false;
    for(int i=0; i < currentPath->count(); i++) {
        PathPoint *pt = &(*currentPath)[i];
        MapPoint *thisMapPt = &(pt->pos);
        bool inbounds = viewCtx.getBoundingBox().contains(*thisMapPt);
        if (i == 0 || pt->time < timeCtx.getMapSeconds()) {
            if (i > 0 && (inbounds || lastInbounds)) {
                _pathBuffer[bufferIndex++] = lastMapPt->x;
                _pathBuffer[bufferIndex++] = lastMapPt->y;
                _pathBuffer[bufferIndex++] = thisMapPt->x;
                _pathBuffer[bufferIndex++] = thisMapPt->y;
            }
            lastMapPt = thisMapPt;
            lastPathPt = pt;
            _particlePos = *thisMapPt;
        } else {
            double elapsed = timeCtx.getMapSeconds() -
                             double(lastPathPt->time);
            int trkElapsed = pt->time - lastPathPt->time;
            double f = (trkElapsed == 0) ? 0. : elapsed / double(trkElapsed);
            if (f > 0.0 && (inbounds || lastInbounds)) {
                MapPoint finalPt = lerp(*lastMapPt, *thisMapPt, f);
                _pathBuffer[bufferIndex++] = lastMapPt->x;
                _pathBuffer[bufferIndex++] = lastMapPt->y;
                _pathBuffer[bufferIndex++] = finalPt.x;
                _pathBuffer[bufferIndex++] = finalPt.y;
                _particlePos = finalPt;
            } else {
                _particlePos = *thisMapPt;
            }
            break;
        }
        lastInbounds = inbounds;
    }
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, _pathBuffer );
    glDrawArrays( GL_LINES, 0, bufferIndex/2 );
    glDisableClientState( GL_VERTEX_ARRAY );
}

void
TrackLayer::_drawParticle(const ViewCtx &viewCtx) const
{
    gl::color( Color( 1, 1, 1 ) );
    float radius = _particleRadius;
    if (radius < viewCtx.getResolution()*2.)
        radius = viewCtx.getResolution()*2.;
    gl::drawSolidCircle( Vec2f( _particlePos.x, _particlePos.y ), radius);
    if (viewCtx.isSelected(id())) {
        gl::drawStrokedCircle( Vec2f(_particlePos.x, _particlePos.y), radius*1.5);
        gl::drawStrokedCircle( Vec2f(_particlePos.x, _particlePos.y), radius*2.0);
    }
    gl::color( Color( 0.3, 0.3, 0.3 ) );
    gl::drawStrokedCircle( Vec2f(_particlePos.x, _particlePos.y), radius);
}

BoundingBox
TrackLayer::getBoundingBox() const
{
    return _bounds;
}

MapPoint
TrackLayer::position() const
{
    return _particlePos;
}

bool
TrackLayer::ephemeral() const
{
    return false;
}
