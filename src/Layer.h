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

// Represents a single map layer

class Layer
{
public:
    
    Layer();
    
    virtual ~Layer();
    
    virtual void draw(const ViewCtx&) = 0;
    
};

#endif
