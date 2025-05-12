#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "light_source.h"

/// A “sun” or “infinite” light: same direction and intensity everywhere.
class directional_light : public light_source {
    vec3   dir;       // direction *from* light *toward* scene (should be normalized)
    color  radiance;  // constant RGB intensity
public:
    /// @param direction  points *from* the light *toward* the scene
    /// @param intensity  RGB radiance (e.g. color(1,1,1) for white)
    directional_light(const vec3& direction, const color& intensity)
      : dir(unit_vector(direction)), radiance(intensity) {}

    vec3  direction(const point3& /*p*/)    const override {
        // always the same
        return -dir;  
    }
    color intensityAt(const point3& /*p*/) const override {
        // no attenuation
        return radiance;
    }
};

#endif