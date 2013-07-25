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

class TimeCtx
{
public:
    
    TimeCtx();
    
    virtual ~TimeCtx();

    void setPlaybackRate(double);
    
    double getPlaybackRate() const;
    
    double update(double);
    
    double getMapSeconds() const;
    
protected:
    
    double _playbackRate;
    
    double _mapSeconds;
    
};

#endif /* defined(__RunParticles__TimeCtx__) */
