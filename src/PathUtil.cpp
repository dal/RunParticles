#include "PathUtil.h"
#include "cinder/Vector.h"

#include "math.h"

using namespace cinder;

float
closestPoint(Vec2d v, Vec2d w, Vec2d p)
{
    // Return minimum distance between line segment vw and point p
    const float l2 = (v-w).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return (v - p).length();   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    const float t = (p - v).dot(w - v) / l2;
    if (t < 0.0) return (v - p).length();       // Beyond the 'v' end of the segment
    else if (t > 1.0) return (w - p).length();  // Beyond the 'w' end of the segment
    const Vec2f projection = v + t * (w - v);  // Projection falls on the segment
    return (projection - p).length();
}

Path
PathUtil::DouglasPeucker(const Path &input, double epsilon)
{
    // Find the point with the maximum distance
    Path result;
    float dmax = 0.;
    int index = 0;
    ulong end = input.count();
    for (ulong i=0; i < end; i++) {
        float d = closestPoint(input[0].pos, input.last().pos, input[i].pos);
        if ( d > dmax ) {
            index = i;
            dmax = d;
        }
    }
    // If max distance is greater than epsilon, recursively simplify
    if ( dmax > epsilon ) {
        // Recurse
        Path recResults1 = DouglasPeucker(input.mid(0, index), epsilon);
        Path recResults2 = DouglasPeucker(input.mid(index, -1), epsilon);
        
        // Build the result list
        return recResults1.mid(0, recResults1.count()-2) + recResults2;
    } else {
        return input;
    }
}

