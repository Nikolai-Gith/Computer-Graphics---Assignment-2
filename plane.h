#ifndef PLANE_H
#define PLANE_H

#include "primitive.h"
#include "vec3.h"

class plane : public primitive {
public:
    // Plane equation: ax + by + cz + d = 0
    plane(double a, double b, double c, double d) : normal(unit_vector(vec3(a, b, c))), d(d) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_struct& hit_out) const override {
        // Plane intersection: t = -(a·o + d) / (a·d)
        double denom = dot(normal, r.direction());
        if (std::abs(denom) < 1e-6) return false; // Ray is parallel to the plane

        double t = -(dot(normal, r.origin()) + d) / denom;
        if (t < ray_tmin || t > ray_tmax) return false;

        hit_out.t = t;
        hit_out.p = r.at(t);
        hit_out.normal = normal; // already normalized in constructor

        return true;
    }

private:
    vec3 normal;
    double d;
};

#endif