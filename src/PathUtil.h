#ifndef PATHUTIL_H
#define PATHUTIL_H

#include "Types.h"

namespace PathUtil
{
    Path DouglasPeucker(const Path &input, double epsilon = 1.0);
};

#endif
