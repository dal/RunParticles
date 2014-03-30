/*
 *  ViewCtx.cpp
 *  RunParticles
 * 
 *  Created by Doug Letterman on 1/31/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "ViewCtx.h"

ViewCtx::ViewCtx()
    : _width(0),
      _height(0),
      _resolution(1.)
{
    
}

ViewCtx::~ViewCtx()
{
    
}

double
ViewCtx::getResolution() const
{
    return _resolution;
}

void
ViewCtx::setViewport(const MapPoint upperLeft, const MapPoint lowerRight,
                     int width, int height)
{
    _upperLeft = upperLeft;
    _lowerRight = lowerRight;
    _width = width;
    _height = height;
    _resolution = (width != 0) ? abs(lowerRight.x - upperLeft.x) / double(width)
                               : 1.0;
}

BoundingBox
ViewCtx::getBoundingBox() const
{
    return BoundingBox(_upperLeft, _lowerRight);
}

bool
ViewCtx::isSelected(const LayerId id) const
{
    return (selectedLayers.find(id) != selectedLayers.end());
}

