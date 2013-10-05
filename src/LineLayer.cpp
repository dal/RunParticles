
#include "LineLayer.h"

#include "cinder/gl/gl.h"

using namespace cinder;

LineLayer::LineLayer() 
{
    // do nothing
}

LineLayer::~LineLayer() 
{
    // do nothing
}

void 
LineLayer::draw(const ViewCtx *ctx, const TimeCtx*)
{
    LatLon a(37.81155, 122.26667);
    LatLon b(37.81941, 122.29423);
    LatLon c(37.87309, 122.30556);
    LatLon d(37.87099, 122.25105);
    
    MapPoint ma = ctx->toProjection(a);
    MapPoint mb = ctx->toProjection(b);
    MapPoint mc = ctx->toProjection(c);
    MapPoint md = ctx->toProjection(d);
    
    gl::drawLine( Vec2f(ma.x, ma.y), Vec2f(mb.x, mb.y));
    gl::drawLine( Vec2f(mb.x, mb.y), Vec2f(mc.x, mc.y));
    gl::drawLine( Vec2f(mc.x, mc.y), Vec2f(md.x, md.y));
    gl::drawLine( Vec2f(md.x, md.y), Vec2f(ma.x, ma.y));
}
