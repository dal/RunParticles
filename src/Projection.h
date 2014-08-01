#ifndef PROJECTION_H
#define PROJECTION_H

#include "Types.h"

class Projection
{
public:
    
    Projection();
    
    virtual ~Projection();
    
    MapPoint toProjection(const LonLat&) const;
    
    LonLat fromProjection(const MapPoint&) const;
    
    float getScaleMultiplier() const;
    
protected:
    
    const std::string _projection;
    
};

#endif
