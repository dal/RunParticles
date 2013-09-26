/*
 *  MapView.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/21/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "MapView.h"

/*
 *  MapView.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/21/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

MapView::MapView() 
{ 
    mInitialCam = mCurrentCam = CameraOrtho(); 
}

MapView::MapView( const CameraOrtho &aInitialCam ) 
{ 
    mInitialCam = mCurrentCam = aInitialCam; 
}

void 
MapView::mouseDown( const Vec2i &mousePos )
{
    mInitialMousePos = mousePos;
    mInitialCam = mCurrentCam;
    mLastAction = ACTION_NONE;
}

void MapView::mouseWheel(const int delta)
{
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    // *left, *top, *right, *bottom, *near, *far
    mCurrentCam.getFrustum(&oldLeft, 
                           &oldTop, 
                           &oldRight, 
                           &oldBottom, 
                           &oldNear, 
                           &oldFar);
    // TODO
    
}

void MapView::mouseDrag(const Vec2i &mousePos, 
                        bool leftDown, 
                        bool middleDown, 
                        bool rightDown)
{
    int action;
    if ( leftDown || ( leftDown && middleDown ) ) {
        action = ACTION_PAN;
    } else if( middleDown || rightDown) {
        action = ACTION_ZOOM;
    } else {
        return;
    }
    
    if( action != mLastAction ) {
        mInitialCam = mCurrentCam;
        mInitialMousePos = mousePos;
    }
    
    mLastAction = action;
    
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    // *left, *top, *right, *bottom, *near, *far
    mCurrentCam.getFrustum(&oldLeft, 
                           &oldTop, 
                           &oldRight, 
                           &oldBottom, 
                           &oldNear, 
                           &oldFar);
    
    if( action == ACTION_ZOOM ) { // zooming
        int mouseDelta = ( mousePos.x - mInitialMousePos.x ) + 
                         ( mousePos.y - mInitialMousePos.y );
        float size = (oldLeft - oldRight + oldTop - oldBottom);
        if (size == 0) size = 1.0;
        float diff = mouseDelta / size / 2.0;
        
        mCurrentCam.setOrtho(oldLeft*diff, 
                             oldRight*diff, 
                             oldBottom*diff, 
                             oldTop*diff, 
                             -1, 
                             1);
    }
    else if( action == ACTION_PAN ) { // panning
        float deltaX = ( mInitialMousePos.x - mousePos.x );
        float deltaY = ( mInitialMousePos.y - mousePos.y );
        // left, float right, float bottom, float top
        mCurrentCam.setOrtho(oldLeft+deltaX, 
                             oldRight+deltaX, 
                             oldBottom+deltaY, 
                             oldTop+deltaY, 
                             -1, 
                             1);
    }
    mInitialMousePos = mousePos;
}
    
const CameraOrtho& MapView::getCamera() const 
{ 
    return mCurrentCam; 
}

void MapView::setCurrentCam( const CameraOrtho &aCurrentCam ) 
{ 
    mCurrentCam = aCurrentCam; 
}

void MapView::zoom( const float amount )
{
    
}
