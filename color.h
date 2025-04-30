#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <vector>

#include "vec3.h"

using color = vec3;

void write_color(std::vector<unsigned char>& out, int index, const color& pixel_color){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out[index + 0] = rbyte;
    out[index + 1] = gbyte;
    out[index + 2] = bbyte;
}

#endif