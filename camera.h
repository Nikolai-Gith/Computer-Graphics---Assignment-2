#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <string>
#include <cmath>

#include "ray.h"
#include "primitive.h"
#include "color.h"
#include "stb_image_write.h"

// camera always looks at center of z=0 plane
// where the right up corner is (1,1,0) and bottom left is (-1,-1,0)
class camera
{
public:
    camera(const point3& origin, int px_height, int px_width, color background)
        : orig(origin), height(px_height), width(px_width), bg_color(background){}
    void render(const std::vector<primitive*>& scene, const std::string& output_file_name){
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
                auto intersection_hit = get_min_intersection(r, scene);
                auto pixel_color = get_color(r, intersection_hit);
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
    hit_struct get_min_intersection(const ray& r, const std::vector<primitive*>& scene) const {
        hit_struct hit_out;
        hit_out.t = INFINITY;  // Start with max t
        hit_struct hit;
    
        for (const auto& hittable : scene) {
            if (hittable->hit(r, 0.001, INFINITY, hit)) {  // 0.001 to avoid shadow acne
                if (hit.t < hit_out.t) {
                    hit_out = hit;
                }
            }
        }
    
        // If no hit occurred, set t to -INFINITY as signal
        if (hit_out.t == INFINITY) {
            hit_out.t = -INFINITY;
        }
    
        return hit_out;
    }

    color get_color(const ray& r, const hit_struct& hit) const {
        if(hit.t > -INFINITY){
            return 0.5 * (hit.normal + color(1,1,1));
        }

        return bg_color;
    }
};

#endif


