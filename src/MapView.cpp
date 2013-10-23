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

MapView::MapView() :
    mCurrentCam(CameraOrtho())
{ 
    
}

MapView::MapView( const CameraOrtho &aInitialCam ) 
{ 
    mCurrentCam = aInitialCam;
}

void MapView::mouseWheel(const int delta)
{
    zoom(delta);
}

void MapView::mouseDrag(const Vec2f &mouseDelta,
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
    
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    mCurrentCam.getFrustum(&oldLeft, 
                           &oldTop, 
                           &oldRight, 
                           &oldBottom, 
                           &oldNear, 
                           &oldFar);
    
    if( action == ACTION_ZOOM ) { // zooming
        float size = (oldLeft - oldRight + oldTop - oldBottom);
        if (size == 0) size = 1.0;
        float diff = (mouseDelta.x + mouseDelta.y) / size / 2.0;
        
        mCurrentCam.setOrtho(oldLeft*diff, 
                             oldRight*diff, 
                             oldBottom*diff, 
                             oldTop*diff, 
                             -1, 
                             1);
    }
    else if( action == ACTION_PAN ) { // panning
        // left, float right, float bottom, float top
        mCurrentCam.setOrtho(oldLeft+mouseDelta.x,
                             oldRight+mouseDelta.x,
                             oldBottom+mouseDelta.y,
                             oldTop+mouseDelta.y,
                             -1, 
                             1);
    }
}

void
MapView::resize(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    mCurrentCam.getFrustum(&oldLeft,
                           &oldTop,
                           &oldRight,
                           &oldBottom,
                           &oldNear,
                           &oldFar);
    float mapWidth = (oldRight - oldLeft);
    float mapHeight = (oldTop - oldBottom);
    float hOffset = mapWidth*(float(newWidth)/float(oldWidth)) - mapWidth;
    float vOffset = mapHeight*(float(newHeight)/float(oldHeight)) - mapHeight;
    mCurrentCam.setOrtho(oldLeft,
                         oldRight+hOffset,
                         oldBottom-vOffset,
                         oldTop,
                         -1,
                         1);
}

const CameraOrtho&
MapView::getCamera() const
{ 
    return mCurrentCam; 
}

void
MapView::setCurrentCam( const CameraOrtho &aCurrentCam )
{ 
    mCurrentCam = aCurrentCam; 
}

void
MapView::zoom( const float amount )
{
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    mCurrentCam.getFrustum(&oldLeft,
                           &oldTop,
                           &oldRight,
                           &oldBottom,
                           &oldNear,
                           &oldFar);
    float size = (amount == 0) ? 1.0 : amount;
    float width = (oldRight - oldLeft);
    float height = (oldTop - oldBottom);
    float hsize = width * size / 50.;
    float vsize = height * size / 50.;
    if (hsize > 0.5 * width)
        return;
    mCurrentCam.setOrtho(oldLeft+hsize,
                         oldRight-hsize,
                         oldBottom+vsize,
                         oldTop-vsize,
                         -1,
                         1);
}

void
MapView::recenter(const MapPoint &position)
{
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    // *left, *top, *right, *bottom, *near, *far
    mCurrentCam.getFrustum(&oldLeft,
                           &oldTop,
                           &oldRight,
                           &oldBottom,
                           &oldNear,
                           &oldFar);
    float width = (oldRight - oldLeft) * 0.5;
    float height = (oldTop - oldBottom) * 0.5;
    float newLeft = position.x - width;
    float newRight = position.x + width;
    float newTop = position.y + height;
    float newBottom = position.y - width;
    mCurrentCam.setOrtho(newLeft,
                         newTop,
                         newRight,
                         newBottom,
                         -1,
                         1);
}

void
MapView::getFrustum(float &left, float &top, float &right, float &bottom) const
{
    float near, far;
    mCurrentCam.getFrustum(&left,
                           &top,
                           &right,
                           &bottom,
                           &near,
                           &far);
}

