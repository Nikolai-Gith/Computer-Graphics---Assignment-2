#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "ray.h"
#include "color.h"
#include "definitions.h"

class primitive;
class hit_struct{
    public:
        point3 p;
        vec3 normal;
        double t;
        primitive* prim;
};

class primitive
{
public:
    virtual ~primitive() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_struct& hit_out) const = 0;
    virtual color get_color_at(const ray&  r, const hit_struct&  hit) const = 0;
    
    const material_t& get_metrial() const { return material; }
    void set_material(const material_t& m) { material = m; }

protected:
    material_t material;
};

#endif

