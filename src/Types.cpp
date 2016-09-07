//
//  Types.cpp
//  RunParticles
//
//  Created by Doug Letterman on 4/24/16.
//
//

#include "Types.h"

quint16 const Track::classVersion = 1;

QDataStream& operator<<(QDataStream &stream, const LonLat& point)
{
    stream << point.x << point.y;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, LonLat& point)
{
    stream >> point.x;
    stream >> point.y;
    return stream;
}

QDataStream& operator<<(QDataStream &stream, const TrackPoint& point)
{
    stream << point.time << point.pos;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, TrackPoint& point)
{
    stream >> point.time;
    stream >> point.pos;
    return stream;
}

QDataStream& operator<<(QDataStream &stream, const Track& track)
{
    stream << Track::classVersion << track.name << track.sport << track.points;
    return stream;
}

QDataStream& operator>>(QDataStream &stream, Track& track)
{
    quint16 thisClassVersion = 0;
    stream >> thisClassVersion;
    // TODO: check for class version from the future
    stream >> track.name;
    stream >> track.sport;
    stream >> track.points;
    return stream;
}

DeferredRequest::DeferredRequest() :
    status(RequestStatusPending)
{
    
}

