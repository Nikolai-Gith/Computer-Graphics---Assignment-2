#include "color.h"
#include "parser.h"
#include "camera.h"
#include "sphere.h"
#include "definitions.h"

#include <iostream>
#include <vector>

int main(){
    int px_height = 256;
    int px_width = 256;

    // Load and parse scene
    parser scene_parser;
    scene_parser.load("scene1.txt");

    //temp
    // sphere sphere_test(point3(1, 0, 0), 0.3);
    // std::vector<primitive*> scene;
    // scene.push_back(&sphere_test);

    // Get scene objects
    auto scene_objects = scene_parser.get_scene_objects();

    // Get light sources
    auto light_sources = scene_parser.get_lights();

    auto ambient = scene_parser.get_ambient();

    // Add to scene
    std::vector<primitive*> scene;
    for (const auto& obj : scene_objects) {
        obj.shape->set_material(obj.material);
        scene.push_back(obj.shape);
    }

    // Camera
    auto camera_center = scene_parser.get_eye();
    camera cam(camera_center, px_height, px_width, color(0, 0, 0)); // black bg
    cam.render(scene, light_sources, ambient, "output.png");

    // Clean up memory
    for (auto* obj : scene) {
        delete obj;
    }
    for (auto* l : light_sources) delete l;
}