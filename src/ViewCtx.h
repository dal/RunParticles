/*
 *  Viewer.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 1/31/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef VIEWCTX_H
#define VIEWCTX_H

#include "Types.h"

#include "TimeCtx.h"

#include <string>

class ViewCtx 
{
    
public:
    ViewCtx();
    
    virtual ~ViewCtx();
    
    const LonLat getUpperLeft();
    
    MapPoint toProjection(const LonLat&);
    
    LonLat fromProjection(const MapPoint&);
    
    const TimeCtx* getTimeCtx() const;

protected:
    LonLat _upperLeft;
    
    const std::string _projection;
    
    TimeCtx *_timeCtx;
    
};

#endif