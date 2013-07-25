/*
 *  Track.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 12/30/12.
 *  Copyright 2012 Doug Letterman. All rights reserved.
 *
 */
#ifndef TRACK_H
#define TRACK_H

#include "Types.h"

#include <QList>
#include <QString>

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

#endif