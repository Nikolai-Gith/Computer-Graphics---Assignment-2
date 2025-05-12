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

// custom utility functions
#include "util.h"

#include <cstdlib> // for rand()
#include <ctime>   // for seeding rand()

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
        // get random value for jittering
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

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
        // auto pixel_upper_left = screen_origin + 0.5 * (pixel_delta_u + pixel_delta_v);
        // Calculate from exact center - for antialiasing to work without shifting
        auto pixel_upper_left = screen_origin;

        // Render
        for (int j = 0; j < height; j++){
            std::cout << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
            for (int i = 0; i < width; i++){
                color pixel_color(0, 0, 0);
                int samples_per_axis = 3;
                double inv_samples = 1.0 / (samples_per_axis * samples_per_axis);

                for (int sy = 0; sy < samples_per_axis; ++sy) {
                    for (int sx = 0; sx < samples_per_axis; ++sx) {
                        double jitter_x = ((double) std::rand() / RAND_MAX);
                        double jitter_y = ((double) std::rand() / RAND_MAX);

                        // Proper per-grid jittered sample
                        double offset_u = (i + (sx + jitter_x) / samples_per_axis);
                        double offset_v = (j + (sy + jitter_y) / samples_per_axis);

                        auto pixel_sample = pixel_upper_left 
                            + offset_u * pixel_delta_u 
                            + offset_v * pixel_delta_v;

                        auto ray_direction = pixel_sample - orig;
                        ray r(orig, ray_direction);
                        auto intersection_hit = get_min_intersection(r, scene, INFINITY);
                        pixel_color += shade(r, intersection_hit, scene, lights, ambient);
                    }
                }
                pixel_color *= inv_samples;
                // Normalize
                pixel_color = clamp(pixel_color, 0.0, 1.0);

                // Gamma correction
                double gamma = 1.0;
                // correct if power != 1
                if(gamma != 1.0)
                    pixel_color = color(
                        pow(pixel_color.x(),gamma),
                        pow(pixel_color.y(),gamma),
                        pow(pixel_color.z(),gamma)
                    );
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


