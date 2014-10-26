#include "TrackLayer.h"

#include <QColor>

#include "PathUtil.h"
#include "Projection.h"
#include "Types.h"
#include "ViewCtx.h"

#define PARTICLE_RADIUS 12.0
#define MEDIUM_LOD_RES 10.0
#define LO_LOD_RES 20.0

using namespace cinder;

const Color TrackLayer::RunColor = Color( 1, 0, 0 );
const Color TrackLayer::OtherColor = Color( 0.3, 0.3, 1 );
const Color TrackLayer::SelectedColor = Color( 1, 1, 0 );

bool TrackLayer::_isSetup = false;
gl::DisplayList TrackLayer::_particle;
gl::DisplayList TrackLayer::_selectedParticle;

void
TrackLayer::_setup()
{
    _particle = gl::DisplayList(GL_COMPILE);
    _particle.newList();
    gl::drawSolidCircle( Vec2d(0.f, 0.f), 10.);
    gl::color( Color( 0.3, 0.3, 0.3 ) );
    gl::drawStrokedCircle( Vec2d(0.f, 0.f), 10.);
    _particle.endList();
    
    _selectedParticle = gl::DisplayList(GL_COMPILE);
    _selectedParticle.newList();
    gl::color(SelectedColor);
    gl::drawStrokedCircle(Vec2d(0.f, 0.f), 15.);
    gl::drawStrokedCircle(Vec2d(0.f, 0.f), 20.);
    _selectedParticle.endList();
    
    _isSetup = true;
}

TrackLayer::TrackLayer(const Track *track) : Layer(),
_particleRadius(PARTICLE_RADIUS),
_mediumLodRes(MEDIUM_LOD_RES),
_loLodRes(LO_LOD_RES),
_track(track),
_duration(0)
{
    if (!_isSetup)
        _setup();
    int numPts = track->points.size();
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
    delete _track;
}

QString
TrackLayer::name() const
{
    return _track->name;
}

QString
TrackLayer::sourceFilePath() const
{
    return _track->sourceFilePath;
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
    layers.insert(Pass_Particles);
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
    for(size_t i=0; i < _track->points.size(); i++) {
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
    // Make the medium res path
    _path_med = PathUtil::DouglasPeucker(_path_hi, _mediumLodRes);
    // Make the low res path
    _path_lo = PathUtil::DouglasPeucker(_path_med, _loLodRes);
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
        case Pass_Particles:
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
    
    MapPoint w2c = viewCtx.getWorldToCamera();
    PathPoint *lastPathPt;
    MapPoint *lastMapPt;
    int bufferIndex = 0;
    bool lastInbounds = false;
    for(int i=0; i < currentPath->count(); i++) {
        PathPoint *pt = &(*currentPath)[i];
        MapPoint *thisMapPt = &(pt->pos);
        bool inbounds = viewCtx.getBoundingBox().expand(_particleRadius*2.)
                                                .contains(*thisMapPt);
        if (i == 0 || pt->time < timeCtx.getMapSeconds()) {
            if (i > 0 && (inbounds || lastInbounds)) {
                _pathBuffer[bufferIndex++] = w2c.x + lastMapPt->x;
                _pathBuffer[bufferIndex++] = w2c.y + lastMapPt->y;
                _pathBuffer[bufferIndex++] = w2c.x + thisMapPt->x;
                _pathBuffer[bufferIndex++] = w2c.y + thisMapPt->y;
            }
            lastMapPt = thisMapPt;
            lastPathPt = pt;
            _particlePos = *lastMapPt;
        } else {
            double elapsed = timeCtx.getMapSeconds() -
                             double(lastPathPt->time);
            int trkElapsed = pt->time - lastPathPt->time;
            double f = (trkElapsed == 0) ? 0. : elapsed / double(trkElapsed);
            if (f > 0.0 && (inbounds || lastInbounds)) {
                MapPoint finalPt = lerp(*lastMapPt, *thisMapPt, f);
                _pathBuffer[bufferIndex++] = w2c.x + lastMapPt->x;
                _pathBuffer[bufferIndex++] = w2c.y + lastMapPt->y;
                _pathBuffer[bufferIndex++] = w2c.x + finalPt.x;
                _pathBuffer[bufferIndex++] = w2c.y + finalPt.y;
                _particlePos = finalPt;
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
TrackLayer::_drawParticle(const ViewCtx &viewCtx)
{
    // transform to camera space
    float radius = _particleRadius * viewCtx.getDevicePixelRatio();
    if (radius < viewCtx.getResolution()*2.)
        radius = viewCtx.getResolution()*2.;
    const Vec2d particlePosCamera = viewCtx.getWorldToCamera() + _particlePos;
    gl::color(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslated(particlePosCamera.x, particlePosCamera.y, 0.0);
    glScalef(radius/10., radius/10., 1.0);
    
    _particle.draw();
    if (viewCtx.isSelected(id())) {
        _selectedParticle.draw();
    }
    glPopMatrix();
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
