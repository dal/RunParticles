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

#include "Types.h"
#include "ViewCtx.h"

/*
 * Encapsulates one camera view onto a map.
 */
class MapView {
public:
    MapView();
    
    void move(const Vec2d &moveVector);
    void resize(int oldWidth, int oldHieght, int newWidth, int newHeight);
    
    void zoom( const float );
    void recenter(const MapPoint &position);
    void frameBoundingBox(const BoundingBox &bbox);
    void getFrustum(double &left, double &top, double &right, double &bottom) const;
    void setFrustum(double left, double top, double right, double bottom);
        
private:
    double _left, _top, _right, _bottom;
    float _aspectRatio;
};

#endif