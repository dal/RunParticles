/*
 *  MapView.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/21/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "MapView.h"

#include <QDebug>

MapView::MapView() :
    mCurrentCam(CameraOrtho()),
    _aspectRatio(1.0)
{ 
    
}

MapView::MapView(CameraOrtho &aInitialCam) :
    mCurrentCam(aInitialCam)
{ 
    
}

void
MapView::resize(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
    _aspectRatio = (float)newWidth / (float)newHeight;
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

void
MapView::move(const Vec2f &moveVector)
{
    float oldLeft, oldTop, oldRight, oldBottom;
    getFrustum(oldLeft, oldTop, oldRight, oldBottom);
    mCurrentCam.setOrtho(oldLeft+moveVector.x,
                         oldRight+moveVector.x,
                         oldBottom+moveVector.y,
                         oldTop+moveVector.y,
                         -1,
                         1);
}

const CameraOrtho&
MapView::getCamera() const
{ 
    return mCurrentCam; 
}

void
MapView::setCurrentCam( CameraOrtho &aCurrentCam )
{ 
    mCurrentCam = aCurrentCam;
    float left, top, right, bottom, near, far;
    mCurrentCam.getFrustum(&left,
                           &top,
                           &right,
                           &bottom,
                           &near,
                           &far);
    _aspectRatio = (right - left) / (top - bottom);
}

void
MapView::zoom(const float size)
{
    float oldLeft, oldTop, oldRight, oldBottom, oldNear, oldFar;
    mCurrentCam.getFrustum(&oldLeft,
                           &oldTop,
                           &oldRight,
                           &oldBottom,
                           &oldNear,
                           &oldFar);
    float width = fabsf(oldRight - oldLeft);
    MapPoint ctr((oldLeft+oldRight)*0.5, (oldBottom+oldTop)*0.5);
    float hsize = width * size * 0.5;
    float vsize = hsize / _aspectRatio;
    mCurrentCam.setOrtho(ctr.x - hsize,
                         ctr.x + hsize,
                         ctr.y - vsize,
                         ctr.y + vsize,
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
    float newBottom = position.y - height;
    mCurrentCam.setOrtho(newLeft,
                         newRight,
                         newBottom,
                         newTop,
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

