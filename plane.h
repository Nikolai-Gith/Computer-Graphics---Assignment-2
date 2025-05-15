#ifndef PLANE_H
#define PLANE_H

#include "primitive.h"
#include "vec3.h"

class plane : public primitive {
public:
    // Plane equation: ax + by + cz + d = 0
    plane(double a, double b, double c, double d)
    {
        vec3 n = vec3(a, b, c);
        this->normal = unit_vector(vec3(a, b, c));
        this->d  = d / n.length(); // normalize d too
    }

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

    color get_color_at(const ray& /*r*/, const hit_struct& hit) const override 
    {
        color base = checkerboard_color(material.ambient, hit.p);
        return base;
    }

private:
    vec3 normal;
    double d;

    color checkerboard_color(const color& rgb, const point3 hitPoint) const {
        const float scale = 0.5f;
        int ix = floor((hitPoint.x() + 1e-6)/scale);
        int iy = floor((hitPoint.y() + 1e-6)/scale);
        bool dark = ((ix+iy)&1)==0;
        return dark ? 0.5f*rgb : rgb;
    }
};

#endif