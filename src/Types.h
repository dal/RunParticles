/*
 *  Types.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/16/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include "cinder/Vector.h"

using namespace cinder;

struct LatLon : public Vec2d
{
    double lon() const { return x; };
    double lat() const { return y; };
};

typedef Vec2d MapPoint;

#endif