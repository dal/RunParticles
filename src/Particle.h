/*
 *  Particle.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 1/30/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include "cinder/gl/gl.h"

#include "Types.h"
#include "ViewCtx.h"

using namespace ci;

class Particle
{
public:
    LonLat loc;
    
    Particle(const LonLat&);
    
    void update();
    
    void draw(const ViewCtx&);
};

#endif