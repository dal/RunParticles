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

struct LonLat
{
    LonLat()  {};
    LonLat(double x_, double y_) : lon(x_), lat(y_) {};
    double lon;
    double lat;
};

struct MapPoint
{
    MapPoint() {};
    MapPoint(double x_, double y_) : x(x_), y(y_) {};
    double x;
    double y;
};

#endif