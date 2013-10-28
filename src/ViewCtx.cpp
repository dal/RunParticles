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

#define MERCATOR_R_MAJ 63781370.
#define HALF_R_MAJ 31890685.

#define PI 3.141592653589793238

double
_xToLon(const double x)
{
    return x * RAD_TO_DEG / 1113194.9079327357;
}

double
_yToLatSph(const double y)
{
    return (1.5707963267948966 -
            (2.0 * atan(exp((-1.0 * y) / MERCATOR_R_MAJ)))) * RAD_TO_DEG;
}

double
_lonToX(const double lon)
{
    return lon * DEG_TO_RAD * 1113194.9079327357;
}

double
_latToYSph(const double lat)
{
    double a = lat * DEG_TO_RAD;
    return HALF_R_MAJ * log((1.0 + sin(a*PI/180.)) / (1.0 - sin(a*PI/180.)));
}

ViewCtx::ViewCtx()
    : _projection("EPSG:3875"),
      _width(0),
      _height(0),
      _resolution(1.)
{
    
}

ViewCtx::~ViewCtx()
{
    
}

MapPoint
ViewCtx::toProjection(const LonLat &lonLat) const
{
    MapPoint projected(_lonToX(lonLat.lon()), _latToYSph(lonLat.lat()));
    return projected;
}

LonLat
ViewCtx::fromProjection(const MapPoint &point) const
{
    LonLat coord(_xToLon(point.x), _yToLatSph(point.y));
    return coord;
}

double
ViewCtx::getResolution() const
{
    return _resolution;
}

void
ViewCtx::setViewport(const MapPoint upperLeft, const MapPoint lowerRight,
                     int width, int height)
{
    _upperLeft = upperLeft;
    _lowerRight = lowerRight;
    _width = width;
    _height = height;
    _resolution = (width != 0) ? abs(lowerRight.x - upperLeft.x) / double(width)
                               : 1.0;
}

BoundingBox
ViewCtx::getBoundingBox() const
{
    return BoundingBox(_upperLeft, _lowerRight);
}

