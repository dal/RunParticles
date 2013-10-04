//
//  Layer.h
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#ifndef RunParticles_Layer_h
#define RunParticles_Layer_h

#include "ViewCtx.h"
#include "Types.h"

// Represents a single map layer

class Layer
{
public:
    
    Layer() {};
    
    virtual ~Layer() {};
    
    virtual void draw(const ViewCtx*, const TimeCtx*) = 0;
    
};

#endif
