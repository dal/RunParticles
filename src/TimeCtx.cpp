//
//  TimeCtx.cpp
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#include "TimeCtx.h"

TimeCtx::TimeCtx()
: _playbackRate(30.0)
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
    _mapSeconds += _playbackRate * elapsed;
    return _mapSeconds;
}

double
TimeCtx::getMapSeconds() const
{
    return _mapSeconds;
}