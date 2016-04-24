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
    _left(0.),
    _top(0.),
    _right(0.),
    _bottom(0.),
    _aspectRatio(1.0)
{ 
    
}

void
MapView::resize(int oldWidth, int /*oldHeight*/, int newWidth, int newHeight)
{
    _aspectRatio = (float)newWidth / (float)newHeight;
    double mapWidth = _right - _left;
    double newMapWidth = mapWidth*(float(newWidth)/float(oldWidth));
    if (newMapWidth > MAX_WIDTH)
        newMapWidth = MAX_WIDTH;
    double newMapHeight = newMapWidth / _aspectRatio;
    _right = _left+newMapWidth;
    _bottom = _top-newMapHeight;
}

void
MapView::move(const Vec2d &moveVector)
{
    _left += moveVector.x;
    _right +=moveVector.x;
    _bottom += moveVector.y;
    _top += moveVector.y;
}

void
MapView::setCurrentCam( CameraOrtho &aCurrentCam )
{
    float left, top, right, bottom, near, far;
    aCurrentCam.getFrustum(&left,
                           &top,
                           &right,
                           &bottom,
                           &near,
                           &far);
    setFrustum(left, top, right, bottom);
    _aspectRatio = (right - left) / (top - bottom);
}

void
MapView::zoom(const float size)
{
    double oldLeft, oldTop, oldRight, oldBottom;
    getFrustum(oldLeft, oldTop, oldRight, oldBottom);
    float width = fabsf(float(oldRight - oldLeft));
    MapPoint ctr((oldLeft+oldRight)*0.5, (oldBottom+oldTop)*0.5);
    float hsize = width * size * 0.5;
    hsize = (hsize*2. > MAX_WIDTH) ? MAX_WIDTH/2. : hsize;
    float vsize = hsize / _aspectRatio;
    setFrustum(ctr.x - hsize,
               ctr.y + vsize,
               ctr.x + hsize,
               ctr.y - vsize);
}

void
MapView::recenter(const MapPoint &position)
{
    double width = (_right - _left) * 0.5;
    double height = (_top - _bottom) * 0.5;
    _left = position.x - width;
    _right = position.x + width;
    _top = position.y + height;
    _bottom = position.y - height;
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
        setFrustum(bbox.upperLeft.x,
                   center.y + viewheight * 0.5,
                   bbox.lowerRight.x,
                   center.y - viewheight * 0.5);
    } else {
        double viewWidth = bbox.height() * _aspectRatio;
        setFrustum(center.x - viewWidth * 0.5,
                   bbox.upperLeft.y,
                   center.x + viewWidth * 0.5,
                   bbox.lowerRight.y);
    }
}

void
MapView::getFrustum(double &left, double &top, double &right, double &bottom) const
{
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
}

void
MapView::setFrustum(double left, double top, double right, double bottom)
{
    _left = left;
    _top = top;
    _right = right;
    _bottom = bottom;
}

