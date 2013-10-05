/*
 *  Particle.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 1/30/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "Particle.h"

Particle::Particle(const LatLon &mloc ) :
    loc(mloc)
{
    
}

void Particle::update() {
    
}

void Particle::draw(const ViewCtx &ctx) {
    gl::drawSolidCircle( Vec2f(50., 50.), 3.0f );
}
