#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "ray.h"
#include "primitive.h"
#include "color.h"
#include "light_source.h"
#include "spotlight.h"

// camera always looks at center of z=0 plane
// where the right up corner is (1,1,0) and bottom left is (-1,-1,0)
class camera
{
public:
    camera(const point3& origin, int px_height, int px_width, color background)
        : orig(origin), height(px_height), width(px_width), bg_color(background){}

    void render(const std::vector<primitive*>& scene, 
                const std::vector<light_source*>& lights,
                const color& ambient,
                const std::string& output_file_name)
    {
        // for output
        std::vector<unsigned char> image(width * height * 3);

        // vectors defining the viewport (-1 to 1)
        auto screen_u = vec3(2.0, 0, 0); 
        auto screen_v = vec3(0, -2.0, 0);
        auto screen_origin = point3(-1, 1, 0);

        // Pixel to pixel distance vectors
        auto pixel_delta_u = screen_u / width;
        auto pixel_delta_v = screen_v / height;

        // Calculate pixel location of upper left pixel
        auto pixel_upper_left = screen_origin + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Render
        for (int j = 0; j < height; j++){
            std::cout << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
            for (int i = 0; i < width; i++){
                auto pixel_center = pixel_upper_left + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - orig;
                ray r(orig, ray_direction);
                auto intersection_hit = get_min_intersection(r, scene, INFINITY);
                auto pixel_color = shade(r, intersection_hit, scene, lights, ambient);
                write_color(image, (j * width + i) * 3, pixel_color);
            }
        }

        stbi_write_png(output_file_name.c_str(), width, height, 3, image.data(), width * 3);
        std::cout << "\rDone.               \n";
    }

private:
    point3 orig;
    int height;
    int width;
    color bg_color;

    // -infinity on hit_out.t means no intersection occured
    hit_struct get_min_intersection(const ray& r, const std::vector<primitive*>& scene, double tmax) const {
        hit_struct best;
        best.t    = tmax;
        best.prim = nullptr;

        hit_struct tmp;
        for (auto* obj : scene) {
            if (obj->hit(r, 0.001, tmax, tmp) && tmp.t < best.t) {
                best = tmp;
                best.prim = obj;
            }
        }

        // If best.prim is still nullptr, there was no hit
        if (!best.prim) {
            best.t = -INFINITY;
        }
        return best;
    }

    color shade(
        const ray& r,
        const hit_struct& hit,
        const std::vector<primitive*>& scene,
        const std::vector<light_source*>& lights,
        const color& ambient
    ) const { 
        if(hit.t == -INFINITY) return bg_color; // hit nothing, get background color

        vec3 P = hit.p;
        vec3 N = hit.normal; // normal is already normalized
        vec3 V = unit_vector(-r.direction()); // view direction
        
        // ambient
        color baseColor = hit.prim->get_color_at(r, hit);
        color result = ambient * baseColor;

        // specular + diffuse with shadows
        color Ks = color(0.7, 0.7, 0.7); // as defined in our instructions
        double shininess = hit.prim->get_metrial().shininess;

        for (auto* L : lights){
            vec3 Ldir = L->direction(P);
            if (dynamic_cast<const plane*>(hit.prim) && dot(N, Ldir) < 0.0) // plane will always face lighting
                N = -N;
            color Li = L->intensityAt(P);

            // check if light hits
            ray shadow_ray(P + N*1e-4, Ldir);
            double tmax = INFINITY;
            if (auto* spot = dynamic_cast<spotlight*>(L))
                tmax = (spot->get_position() - P).length(); // if light is spot light, check intersections up until light source
            
            hit_struct shadow_ray_hit = get_min_intersection(shadow_ray, scene, tmax);
            if (shadow_ray_hit.t > -INFINITY)
                continue; // object in way, no light (Si = 0)
            
            // diffuse
            double NdotL = std::max(dot(N, Ldir), 0.0);
            result += baseColor * Li * NdotL;

            // specular
            vec3 R = -Ldir - 2 * dot(-Ldir, N) * N; // reflected ray direction
            double RdotV = std::max(dot(R,V), 0.0);
            result += Ks * std::pow(RdotV, shininess) * Li;
        }

        return color(std::min(result.x(), 1.0), std::min(result.y(), 1.0), std::min(result.z(), 1.0));
    }
};

#endif


