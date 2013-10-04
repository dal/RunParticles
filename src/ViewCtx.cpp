/*
 *  Viewer.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 1/31/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "ViewCtx.h"
#include <math.h>

#define RAD_TO_DEG	57.29577951308232
#define DEG_TO_RAD	.0174532925199432958

#define MERCATOR_R_MAJ 6378137.0

#define PI 3.141592653589793238

double _xToLon(const double x)
{
    return (RAD_TO_DEG * (x / MERCATOR_R_MAJ));
}

double _yToLatSph(const double y)
{
    return (180./PI * (2 * atan(exp(y*PI/180.)) - PI/2));
}

double _lonToX(const double lon)
{
    return (MERCATOR_R_MAJ*DEG_TO_RAD*lon);
}

double _latToYSph(const double lat)
{
    return (180./PI * log(tan(PI/4+lat*(PI/180.)/2)));
}

ViewCtx::ViewCtx()
    : _projection("EPSG:3875")
{
    
}

ViewCtx::~ViewCtx()
{
    
}

MapPoint
ViewCtx::toProjection(const LonLat &lonLat) const
{
    MapPoint projected(_lonToX(lonLat.lon), _latToYSph(lonLat.lat));
    return projected;
}

LonLat
ViewCtx::fromProjection(const MapPoint &point) const
{
    LonLat coord(_xToLon(point.x), _yToLatSph(point.y));
    return coord;
}

const TimeCtx*
ViewCtx::getTimeCtx() const
{
    return _timeCtx;
}
