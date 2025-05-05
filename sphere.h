#ifndef SPHERE_H
#define SPHERE_H

#include "primitive.h"
#include "vec3.h"

class sphere : public primitive {
    public:
        sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}
        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_struct& hit_out) const override {
            // solving quadratic equation for hitting sphere with b = -2h
            // simplifies to (h+- sqrt(h^2 - ac)) / a
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared(); // r dot r
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;
            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;
            
            auto sqrtd = std::sqrt(discriminant);
            auto root = ((h - sqrtd) / a);
            if (root <= ray_tmin || ray_tmax <= root) {
                root = (h + sqrtd) / a;
                if (root <= ray_tmin || ray_tmax <= root)
                    return false;
            }

            hit_out.t = root;
            hit_out.p = r.at(root);
            hit_out.normal = (hit_out.p - center) / radius;

            return true;
        }
    private:
        point3 center;
        double radius;
};

#endif