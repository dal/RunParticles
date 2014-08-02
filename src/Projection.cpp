
#include "Projection.h"

#include <math.h>

#define RAD_TO_DEG	57.29577951308232
#define DEG_TO_RAD	.0174532925199432958

#define MERCATOR_R_MAJ 6378137.0
#define HALF_R_MAJ 3189068.5

// scale the map domain down by this amount
// this helps prevent floating point precision artifacts
#define SCALE_MULT 1.0

#define PI 3.141592653589793238

double
_xToLon(const double x)
{
    return x * RAD_TO_DEG / MERCATOR_R_MAJ;
}

double
_yToLatSph(const double y)
{
    return ((1.5707963267948966 -
            (2.0 * atan(exp((-1.0 * y) / MERCATOR_R_MAJ)))) * RAD_TO_DEG)
            / SCALE_MULT;
}

double
_lonToX(const double lon)
{
    return SCALE_MULT * lon * DEG_TO_RAD * MERCATOR_R_MAJ;
}

double
_latToYSph(const double lat)
{
    double sina = sin(lat * DEG_TO_RAD);
    return SCALE_MULT * HALF_R_MAJ * log((1.0 + sina) / (1.0 - sina));
}

Projection::Projection() :
    _projection("EPSG:3875")
{
    
}

Projection::~Projection()
{
    
}

MapPoint
Projection::toProjection(const LonLat &lonLat) const
{
    MapPoint projected(_lonToX(lonLat.lon()), _latToYSph(lonLat.lat()));
    return projected;
}

LonLat
Projection::fromProjection(const MapPoint &point) const
{
    LonLat coord(_xToLon(point.x), _yToLatSph(point.y));
    return coord;
}

float
Projection::getScaleMultiplier() const
{
    return SCALE_MULT;
}
