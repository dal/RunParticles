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
    : _cameraToWorld(0, 0),
      _width(0),
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
    double mapWidth = abs(lowerRight.x - upperLeft.x);
    _resolution = (width != 0) ? mapWidth / double(width)
                               : 1.0;
}

void
ViewCtx::setCameraToWorld(MapPoint c2w)
{
    _cameraToWorld = c2w;
}

MapPoint
ViewCtx::getWorldToCamera() const
{
    return -_cameraToWorld;
}

MapPoint
ViewCtx::getCameraToWorld() const
{
    return _cameraToWorld;
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

