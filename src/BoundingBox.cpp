#include "BoundingBox.h"

BoundingBox::BoundingBox(MapPoint upperL, MapPoint lowerR)
    : upperLeft(upperL),
    lowerRight(lowerR),
    _valid(true)
{

}

BoundingBox::BoundingBox(double left, double top, double right, double bottom)
    : _valid(true)
{
    upperLeft.x = left;
    upperLeft.y = top;
    lowerRight.x = right;
    lowerRight.y = bottom;
}

bool
BoundingBox::overlaps(const BoundingBox& other) const
{
    if (!other._valid)
        return false;
    
    return ((upperLeft.x <= other.lowerRight.x 
             && lowerRight.x >= other.upperLeft.x) 
           && (upperLeft.y >= other.lowerRight.y 
             && lowerRight.y <= other.upperLeft.y));
}

bool BoundingBox::contains(const MapPoint& pt) const
{
    if (!_valid)
        return false;
    
    return (pt.x >= upperLeft.x && pt.x <= lowerRight.x &&
            pt.y >= lowerRight.y && pt.y <= upperLeft.y);
}

BoundingBox&
BoundingBox::operator+=(const MapPoint pt)
{
    if (_valid) {
        if (pt.x <= upperLeft.x)
            upperLeft.x = pt.x;
        else if (pt.x >= lowerRight.x)
            lowerRight.x = pt.x;
        if (pt.y >= upperLeft.y)
            upperLeft.y = pt.y;
        else if (pt.y <= lowerRight.y)
            lowerRight.y = pt.y;
    } else {
        upperLeft = pt;
        lowerRight = pt;
        _valid = true;
    }
    return *this;
}

BoundingBox&
BoundingBox::operator+=(const BoundingBox other)
{
    if (other._valid) {
        if (!_valid) {
            upperLeft = other.upperLeft;
            lowerRight = other.lowerRight;
            _valid = true;
        } else {
            *this+= other.upperLeft;
            *this+= other.lowerRight;
        }
    }
    return *this;
}

BoundingBox
BoundingBox::expand(double amt)
{
    return BoundingBox(MapPoint(upperLeft.x-amt, upperLeft.y+amt),
                       MapPoint(lowerRight.x+amt, lowerRight.y-amt));
}

double
BoundingBox::width() const
{
    return lowerRight.x - upperLeft.x;
}

double
BoundingBox::height() const
{
    return upperLeft.y - lowerRight.y;
}

MapPoint
BoundingBox::center() const
{
    return MapPoint((lowerRight.x + upperLeft.x) * 0.5,
                    (upperLeft.y + lowerRight.y) * 0.5);
}

