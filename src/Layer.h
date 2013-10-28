//
//  Layer.h
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#ifndef RunParticles_Layer_h
#define RunParticles_Layer_h

#include "BoundingBox.h"
#include "ViewCtx.h"
#include "Types.h"

// Represents a single map layer

class Layer
{
public:
    
    Layer() {};
    
    virtual ~Layer() {};
    
    virtual unsigned int passes() const { return 1; }
    
    virtual unsigned int duration() const { return 0; }
    
    virtual void project(const ViewCtx*) = 0;
    
    virtual void draw(uint pass, const ViewCtx*, const TimeCtx*) = 0;
    
    virtual BoundingBox getBoundingBox() const = 0;
    
};

#endif
