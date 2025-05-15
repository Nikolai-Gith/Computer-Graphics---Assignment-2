#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <vector>

#include "vec3.h"
#include "util.h"

using color = vec3;

inline color clamp(const color& c, double min, double max) {
    return color(
        clamp(c.x(), min, max),
        clamp(c.y(), min, max),
        clamp(c.z(), min, max)
    );
}

void write_color(std::vector<unsigned char>& out, int index, const color& pixel_color){
    color clamped_color = clamp(pixel_color, 0.0, 1.0);
    auto r = clamped_color.x();
    auto g = clamped_color.y();
    auto b = clamped_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out[index + 0] = rbyte;
    out[index + 1] = gbyte;
    out[index + 2] = bbyte;
}

#endif