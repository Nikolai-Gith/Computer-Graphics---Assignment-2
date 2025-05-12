#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "vec3.h"
#include "color.h"

class light_source {
public:
    /// @param p  the surface point being shaded
    /// @return   unit vector pointing *toward* the light source
    virtual vec3  direction(const point3& p)    const = 0;
    /// @param p  the surface point being shaded
    /// @return   the RGB radiance arriving at p (including any spotlight fall-off or attenuation)
    virtual color intensityAt(const point3& p) const = 0;
    virtual ~light_source() = default;
};

#endif