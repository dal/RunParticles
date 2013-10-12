/*
 *  MapView.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/21/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "Types.h"

using namespace cinder;

class MapView {
public:
    MapView();
    MapView( const CameraOrtho&);
    
    void mouseWheel(const int);
    
    void mouseDrag(const Vec2i &mousePos, 
                   bool leftDown, 
                   bool middleDown, 
                   bool rightDown );
        
    const CameraOrtho& getCamera() const;
    void setCurrentCam( const CameraOrtho &aCurrentCam );
    void zoom( const float );
    void recenter(const MapPoint &position);
        
private:
    enum		{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN };
    
    CameraOrtho	mCurrentCam;
    int			mLastAction;
};

#endif