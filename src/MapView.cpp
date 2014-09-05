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

#define MAX_WIDTH 1.6e+07

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
MapView::resize(int oldWidth, int /*oldHeight*/, int newWidth, int newHeight)
{
    _aspectRatio = (float)newWidth / (float)newHeight;
    float oldLeft, oldTop, oldRight, oldBottom;
    getFrustum(oldLeft, oldTop, oldRight, oldBottom);
    float mapWidth = (oldRight - oldLeft);
    float newMapWidth = mapWidth*(float(newWidth)/float(oldWidth));
    if (newMapWidth > MAX_WIDTH)
        newMapWidth = MAX_WIDTH;
    float newMapHeight = newMapWidth / _aspectRatio;
    mCurrentCam.setOrtho(oldLeft,
                         oldLeft+newMapWidth,
                         oldTop-newMapHeight,
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
    float oldLeft, oldTop, oldRight, oldBottom;
    getFrustum(oldLeft, oldTop, oldRight, oldBottom);
    float width = fabsf(oldRight - oldLeft);
    MapPoint ctr((oldLeft+oldRight)*0.5, (oldBottom+oldTop)*0.5);
    float hsize = width * size * 0.5;
    hsize = (hsize*2. > MAX_WIDTH) ? MAX_WIDTH/2. : hsize;
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
    float oldLeft, oldTop, oldRight, oldBottom;
    getFrustum(oldLeft, oldTop, oldRight, oldBottom);
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
MapView::frameBoundingBox(const BoundingBox &bbox)
{
    MapPoint center = bbox.center();
    float bboxWidth = bbox.width();
    if (bboxWidth > MAX_WIDTH)
        bboxWidth = MAX_WIDTH;
    double bboxRatio = bboxWidth / bbox.height();
    if (bboxRatio > _aspectRatio) {
        double viewheight = bbox.width() / _aspectRatio;
        mCurrentCam.setOrtho(bbox.upperLeft.x,
                             bbox.lowerRight.x,
                             center.y - viewheight * 0.5,
                             center.y + viewheight * 0.5,
                             -1,
                             1);
    } else {
        float viewWidth = bbox.height() * _aspectRatio;
        mCurrentCam.setOrtho(center.x - viewWidth * 0.5,
                             center.x + viewWidth * 0.5,
                             bbox.lowerRight.y,
                             bbox.upperLeft.y,
                             -1,
                             1);
    }
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

