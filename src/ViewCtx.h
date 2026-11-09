/*
 *  ViewCtx.h
 *  RunParticles
 *
 *  Default View Context for RunParticles. Implements "WGS84 Web Mercator" 
 *  projection, AKA EPSG:3857
 * 
 *  See: http://spatialreference.org/ref/sr-org/7483/
 *
 *  Created by Doug Letterman on 1/31/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef VIEWCTX_H
#define VIEWCTX_H

#include "Types.h"
#include "Layer.h"

#include "BoundingBox.h"
#include "TimeCtx.h"

#include <QMatrix4x4>

#include <string>
#include <set>

class ViewCtx 
{
    
public:
    ViewCtx();
    
    virtual ~ViewCtx();
    
    double getResolution() const;
    
    double getDevicePixelRatio() const;
    
    void setDevicePixelRatio(double ratio);
    
    BoundingBox getBoundingBox() const;
    
    void setViewport(const MapPoint upperLeft, const MapPoint lowerRight,
                     int width, int height);
    
    void setCameraToWorld(MapPoint c2w);
    
    MapPoint getWorldToCamera() const;
    
    MapPoint getCameraToWorld() const;
    
    int getViewportWidth() const { return _width; }
    
    int getViewportHeight() const { return _height; }
    
    /* Orthographic projection from camera space to clip space. */
    const QMatrix4x4& getProjection() const { return _projection; }
    
    void setProjection(const QMatrix4x4 &projection)
    {
        _projection = projection;
    }
    
    bool isSelected(const LayerId id) const;
    
    std::set<LayerId> selectedLayers;

protected:
    
    MapPoint _upperLeft, _lowerRight;
    
    MapPoint _cameraToWorld;
    
    int _width, _height;
    
    double _resolution;
    
    double _devicePixelRatio;
    
    QMatrix4x4 _projection;
    
};

#endif