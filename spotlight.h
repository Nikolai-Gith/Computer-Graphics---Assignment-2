#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light_source.h"

/// A point‐light with a cone.  Outside the cone it gives zero, inside it can also fall off by angle.
class spotlight : public light_source {
    point3 position;   
    vec3   dir;        
    double cutoff;     
    color  radiance;   
public:
    /// @param pos        light position
    /// @param direction  cone axis, *from* light *toward* scene
    /// @param cosAngle   dot(L, dir) must ≥ cosAngle to be “inside” the spot
    /// @param intensity  RGB radiance at the center
    spotlight(const point3& pos,
              const vec3&   direction,
              double        cosAngle,
              const color&  intensity)
      : position(pos),
        dir(unit_vector(direction)),
        cutoff(cosAngle),
        radiance(intensity)
    {}

    vec3 direction(const point3& p) const override {
        // incoming light vector at p
        return unit_vector(position - p);
    }

    color intensityAt(const point3& p) const override {
        vec3 L = unit_vector(p - position);       // direction to light
        double spotCos = dot(L, dir);            // alignment with cone axis
        if (spotCos < cutoff) 
            return color(0,0,0);                 // outside cone

        return radiance;
    }

    point3 get_position() { return position; }
};

#endif