
#include "LineLayer.h"

#include "cinder/gl/gl.h"

LineLayer::LineLayer() 
{
    // do nothing
}

LineLayer::~LineLayer() 
{
    // do nothing
}

void 
LineLayer::draw(const ViewCtx &ctx)
{
    LonLat a(37.81155, 122.26667);
    LonLat b(37.81941, 122.29423);
    LonLat c(37.87309, 122.30556);
    LonLat d(37.87099, 122.25105);
    
    gl::drawLine( Vec2f(0, 0), Vec2f(200, 200));
}
