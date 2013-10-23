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
    
    MapPoint toProjection(const LonLat&) const;
    
    LonLat fromProjection(const MapPoint&) const;
    
    const TimeCtx* getTimeCtx() const;
    
    double getResolution() const;
    
    void setViewport(const MapPoint upperLeft, const MapPoint lowerRight,
                     int width, int height);
    
    int getViewportWidth() const { return _width; }
    
    int getViewportHeight() const { return _height; }

protected:
    
    const std::string _projection;
    
    TimeCtx *_timeCtx;
    
    MapPoint _upperLeft, _lowerRight;
    
    int _width, _height;
    
    double _resolution;
    
};

#endif