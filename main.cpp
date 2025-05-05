#include "color.h"
#include "parser.h"
#include "camera.h"
#include "sphere.h"

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

int main(){
    int px_height = 256;
    int px_width = 256;

    // Load and parse scene
    parser scene_parser;
    scene_parser.load("scene1.txt");

    //temp
    sphere sphere_test(point3(1, 0, 0), 0.5);
    std::vector<primitive*> scene;
    scene.push_back(&sphere_test);

    // Camera
    auto camera_center = scene_parser.get_eye();
    camera cam(camera_center, px_height, px_width, color(0, 0, 0)); // black bg
    cam.render(scene, "output.png");
}