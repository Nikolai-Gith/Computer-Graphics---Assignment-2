#include "vec3.h"
#include "color.h"
#include "primitive.h"

struct light_t {
    vec3 direction;
    color intensity;
};

struct spotlight_t {
    vec3 direction;
    point3 position;
    float cutoff;
    color intensity;
};

struct material_t {
    color ambient;
    color diffuse;
    float shininess;
};

struct scene_object {
    primitive* shape;
    material_t material;
};
