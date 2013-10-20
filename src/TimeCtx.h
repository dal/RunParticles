//
//  TimeCtx.h
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#ifndef __RunParticles__TimeCtx__
#define __RunParticles__TimeCtx__

#include <iostream>

/// Class that represents a time context. Maps real time and playback to map
/// time.
class TimeCtx
{
public:
    
    TimeCtx();
    
    virtual ~TimeCtx();

    void setPlaybackRate(double);
    
    double getPlaybackRate() const;
    
    double update(double);
    
    double getMapSeconds() const;
    
    void setMapSeconds(double);
    
protected:
    
    double _playbackRate;
    
    double _mapSeconds;
    
    double _lastTime;
    
};

#endif /* defined(__RunParticles__TimeCtx__) */
