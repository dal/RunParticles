#include "PathUtil.h"
#include "Types.h"

#include "math.h"


float
closestPoint(Vec2d v, Vec2d w, Vec2d p)
{
    // Return minimum distance between line segment vw and point p
    const double l2 = glm::dot(v-w, v-w);  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return glm::length(v - p);   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    const double t = glm::dot(p - v, w - v) / l2;
    if (t < 0.0) return glm::length(v - p);       // Beyond the 'v' end of the segment
    else if (t > 1.0) return glm::length(w - p);  // Beyond the 'w' end of the segment
    const Vec2d projection = v + t * (w - v);  // Projection falls on the segment
    return glm::length(projection - p);
}

Path
PathUtil::DouglasPeucker(const Path &input, double epsilon)
{
    ulong end = input.count();
    if (end <= 2)
        return input;
    Path result;
    float dmax = 0.;
    int index = 0;
    // Find the point with the maximum distance
    for (ulong i=1; i < end - 1; i++) {
        float d = closestPoint(input[0].pos, input.last().pos, input[i].pos);
        if ( d > dmax ) {
            index = i;
            dmax = d;
        }
    }
    // If max distance is greater than epsilon, recursively simplify
    if ( dmax > epsilon ) {
        // Recurse
        Path recResults1 = DouglasPeucker(input.mid(0, index+1), epsilon);
        Path recResults2 = DouglasPeucker(input.mid(index, -1), epsilon);
        
        // Build the result list
        return recResults1.mid(0, recResults1.count()-1) + recResults2;
    } else {
        Path ends;
        ends.append(input.first());
        ends.append(input.last());
        return ends;
    }
}

