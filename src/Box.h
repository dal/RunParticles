//
//  Box.h
//  RunParticles
//
//  Created by Doug Letterman on 11/21/14.
//
//

#ifndef RunParticles_Box_h
#define RunParticles_Box_h

template <class T>
class Box
{
public:
    Box() : _valid(false)
    {
    
    };
    
    Box(T upperL, T lowerR) :
        upperLeft(upperL),
        lowerRight(lowerR),
        _valid(true)
    {
        
    };
    
    Box(double left, double top, double right, double bottom)
    {
        upperLeft.x = left;
        upperLeft.y = top;
        lowerRight.x = right;
        lowerRight.y = bottom;
    };
    
    bool valid() const { return _valid && width() > 0 && height() > 0; }
    
    bool overlaps(const Box<T>& other) const
    {
        if (!other._valid)
            return false;
        
        return ((upperLeft.x <= other.lowerRight.x
                 && lowerRight.x >= other.upperLeft.x)
                && (upperLeft.y >= other.lowerRight.y
                    && lowerRight.y <= other.upperLeft.y));
    };
    
    bool contains(const T& pt) const
    {
        if (!_valid)
            return false;
        
        return (pt.x >= upperLeft.x && pt.x <= lowerRight.x &&
                pt.y >= lowerRight.y && pt.y <= upperLeft.y);
    };
    
    Box& operator+=(const T pt)
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
    };
    
    Box& operator+=(const Box<T> other)
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
    };
    
    Box expand(double amt)
    {
        return Box<T>(T(upperLeft.x-amt, upperLeft.y+amt),
                      T(lowerRight.x+amt, lowerRight.y-amt));
    };
    
    double width() const
    {
        return lowerRight.x - upperLeft.x;
    };
    
    double height() const
    {
        return upperLeft.y - lowerRight.y;
    };
    
    T center() const
    {
        return T((lowerRight.x + upperLeft.x) * 0.5,
                 (upperLeft.y + lowerRight.y) * 0.5);
    };
    
    T upperLeft, lowerRight;
    
protected:
    bool _valid;
};

#endif
