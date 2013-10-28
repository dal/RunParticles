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

#include <QList>
#include <QString>

using namespace cinder;

class LonLat : public Vec2d
{
public:
    LonLat() : Vec2d(0., 0.) { };
    LonLat(double x, double y) : Vec2d(x, y) { };
    double lon() const { return x; };
    double lat() const { return y; };
};

typedef Vec2d MapPoint;

struct TrackPoint
{
    LonLat pos;
    unsigned int time;
};

struct Track
{
    QString sport;
    QList<TrackPoint> points;
};

struct PathPoint
{
    MapPoint pos;
    unsigned int time;
};

typedef QList<PathPoint> Path;

#endif