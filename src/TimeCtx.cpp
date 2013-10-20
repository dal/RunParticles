//
//  TimeCtx.cpp
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#include "TimeCtx.h"

TimeCtx::TimeCtx()
: _playbackRate(1.0),
_lastTime(0.)
{
    
}

TimeCtx::~TimeCtx()
{
    
}

void
TimeCtx::setPlaybackRate(double rate)
{
    _playbackRate = rate;
}

double
TimeCtx::getPlaybackRate() const
{
    return _playbackRate;
}

double
TimeCtx::update(double elapsed)
{
    _mapSeconds += _playbackRate * (elapsed / 1000.);
    return _mapSeconds;
}

double
TimeCtx::getMapSeconds() const
{
    return _mapSeconds;
}

void
TimeCtx::setMapSeconds(double mapSeconds)
{
    _mapSeconds = mapSeconds;
}
