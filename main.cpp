#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "parser.h"

#include <iostream>
#include <vector>

vec3 checkerboardColor(const color& rgbColor, const vec3& hitPoint) {
    float scaleParameter = 0.5f;
    float checkerboard = 0;

    if (hitPoint.x() < 0) {
        checkerboard += std::floor((0.5f - hitPoint.x()) / scaleParameter);
    } else {
        checkerboard += std::floor(hitPoint.x() / scaleParameter);
    }

    if (hitPoint.y() < 0) {
        checkerboard += std::floor((0.5f - hitPoint.y()) / scaleParameter);
    } else {
        checkerboard += std::floor(hitPoint.y() / scaleParameter);
    }

    checkerboard = (checkerboard * 0.5f) - int(checkerboard * 0.5f);
    checkerboard *= 2;

    if (checkerboard > 0.5f) {
        return 0.5f * rgbColor;
    }

    return rgbColor;
}

color ray_color(const ray& r) {
    // Intersect ray with z=0 plane
    double t = -r.origin().z() / r.direction().z();
    point3 hitPoint = r.at(t);

    vec3 baseColor(0.8, 0.8, 0.8);  // light gray

    return checkerboardColor(baseColor, hitPoint);
}

int main(){
    int image_width = 256;
    int image_height = 256;

    std::vector<unsigned char> image(image_width * image_height * 3);

    // Load and parse scene
    parser scene_parser;
    scene_parser.load("scene1.txt");

    // Camera
    auto camera_center = scene_parser.get_eye();

    // Vectors defining the viewport
    auto screen_u = vec3(2.0, 0, 0); 
    auto screen_v = vec3(0, -2.0, 0);
    auto screen_origin = point3(-1, 1, 0);

    // Pixel to pixel distance vectors
    auto pixel_delta_u = screen_u / image_width;
    auto pixel_delta_v = screen_v / image_height;

    // Calculate pixel location of upper left pixel
    auto pixel_upper_left = screen_origin + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    for (int j = 0; j < image_height; j++){
        std::cout << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++){
            auto pixel_center = pixel_upper_left + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);
            // ray intersections check go here
            auto pixel_color = ray_color(r); // temp
            write_color(image, (j * image_width + i) * 3, pixel_color);
        }
    }

    stbi_write_png("output.png", image_width, image_height, 3, image.data(), image_width * 3);
    std::cout << "\rDone.               \n";
}