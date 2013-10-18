

#include "TrackLayer.h"

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
    MapPoint *last = NULL;
    gl::color( Color( 1, 0, 0 ) );
    foreach(pt, _track->points) {
        MapPoint thisMapPt = ctx->toProjection(pt.pos);
        if (last) {
            gl::drawLine( Vec2f(last->x, last->y),
                          Vec2f(thisMapPt.x, thisMapPt.y));
        }
        last = &thisMapPt;
    }
}

