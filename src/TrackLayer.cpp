

#include "TrackLayer.h"
#include "Types.h"

#include "cinder/gl/gl.h"

using namespace cinder;

TrackLayer::TrackLayer(const Track *track) :
_track(track)
{
    // do nothing
}

TrackLayer::~TrackLayer() 
{
    // do nothing
}

void 
TrackLayer::draw(const ViewCtx *ctx, const TimeCtx*)
{
    TrackPoint pt;
    MapPoint last;
    gl::color( Color( 1, 0, 0 ) );
    bool first = true;
    foreach(pt, _track->points) {
        MapPoint thisMapPt = ctx->toProjection(pt.pos);
        if (!first) {
            gl::drawLine( Vec2f(last.x, last.y),
                          Vec2f(thisMapPt.x, thisMapPt.y));
        }
        last = thisMapPt;
        first = false;
    }
    double left, top, right, bottom;
    boundingBox(left, top, right, bottom);
    LatLon upperLeft(top, left);
    LatLon upperRight(top, right);
    LatLon lowerRight(bottom, right);
    LatLon lowerLeft(bottom, left);
    MapPoint ptUl = ctx->toProjection(upperLeft);
    MapPoint ptUr = ctx->toProjection(upperRight);
    MapPoint ptLr = ctx->toProjection(lowerRight);
    MapPoint ptLl = ctx->toProjection(lowerLeft);
    gl::drawLine(Vec2f(ptUl.x, ptUl.y), Vec2f(ptUr.x, ptUr.y));
    gl::drawLine(Vec2f(ptUr.x, ptUr.y), Vec2f(ptLr.x, ptLr.y));
    gl::drawLine(Vec2f(ptLr.x, ptLr.y), Vec2f(ptLl.x, ptLl.y));
    gl::drawLine(Vec2f(ptLl.x, ptLl.y), Vec2f(ptUl.x, ptUl.y));
}

void
TrackLayer::boundingBox(double &left,
                         double &top,
                         double &right,
                         double &bottom)
{
    left = 180.;
    top = -90.;
    right = -180.;
    bottom = 90.;
    TrackPoint pt;
    foreach(pt, _track->points) {
        if (pt.pos.lon > right)
            right = pt.pos.lon;
        if (pt.pos.lon < left)
            left = pt.pos.lon;
        if (pt.pos.lat > top)
            top = pt.pos.lat;
        if (pt.pos.lat < bottom)
            bottom = pt.pos.lat;
    }
}

