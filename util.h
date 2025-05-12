#ifndef UTIL
#define UTIL

#include "color.h"

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline color clamp(const color& c, double min, double max) {
    return color(
        clamp(c.x(), min, max),
        clamp(c.y(), min, max),
        clamp(c.z(), min, max)
    );
}

#endif