#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Types.h"

struct BoundingBox
{
    BoundingBox() { };
    
    BoundingBox(MapPoint upperL, MapPoint lowerR);
    
    BoundingBox(double left, double top, double right, double bottom);
    
    bool overlaps(const BoundingBox& other) const;
    
    bool contains(const MapPoint& pt) const;
    
    BoundingBox& operator+=(const MapPoint pt);
    
    double width() const;
    
    double height() const;
    
    MapPoint center() const;
    
    MapPoint upperLeft, lowerRight;
};

#endif
