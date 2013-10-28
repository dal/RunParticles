#include "BoundingBox.h"

BoundingBox::BoundingBox(MapPoint upperL, MapPoint lowerR)
    : upperLeft(upperL),
    lowerRight(lowerR)
{

}

BoundingBox::BoundingBox(double left, double top, double right, double bottom)
{
    upperLeft.x = left;
    upperLeft.y = top;
    lowerRight.x = right;
    lowerRight.y = bottom;
}

bool
BoundingBox::overlaps(const BoundingBox& other) const
{
    return ((upperLeft.x <= other.lowerRight.x 
             && lowerRight.x >= other.upperLeft.x) 
           && (upperLeft.y >= other.lowerRight.y 
             && lowerRight.y <= other.upperLeft.y));
}

BoundingBox&
BoundingBox::operator+=(const MapPoint pt)
{
    if (pt.x <= upperLeft.x)
        upperLeft.x = pt.x;
    else if (pt.x >= lowerRight.x)
        lowerRight.x = pt.x;
    if (pt.y >= upperLeft.y)
        upperLeft.y = pt.y;
    else if (pt.y <= lowerRight.y)
        lowerRight.y = pt.y;
    return *this;
}

