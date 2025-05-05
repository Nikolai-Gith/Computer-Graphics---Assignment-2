#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "ray.h"

class hit_struct{
    public:
        point3 p;
        vec3 normal;
        double t;
};

class primitive
{
public:
    virtual ~primitive() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_struct& hit_out) const = 0;
};

#endif

