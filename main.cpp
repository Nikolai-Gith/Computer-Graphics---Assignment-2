#include "color.h"
#include "parser.h"
#include "camera.h"
#include "sphere.h"
#include "definitions.h"

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <scene_name_without_extension>\n";
        return 1;
    }

    std::string input_name = argv[1];
    std::string scene_file = input_name + ".txt";
    std::string output_file = "output_" + input_name + ".png";

    int px_height = 256;
    int px_width = 256;

    // Load and parse scene
    parser scene_parser;
    scene_parser.load(scene_file);

    // Get scene objects
    auto scene_objects = scene_parser.get_scene_objects();
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
    cam.render(scene, light_sources, ambient, output_file);

    // Clean up memory
    for (auto* obj : scene) delete obj;
    for (auto* l : light_sources) delete l;

    return 0;
}
