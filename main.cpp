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

    // Default values
    int aa_samples = 1;
    int px_height = 256;
    int px_width = 256;
    double gamma = 1.0;

    // Optional - Get Samples count
    if (argc >= 3) {
        try {
            aa_samples = std::stoi(argv[2]);
            std::cout << "Got AA samples: " << aa_samples << "\n";
        } catch (...) {
            std::cerr << "Invalid AA sample count, using default.\n";
        }
    }

    // Optional - Get resolution
    if (argc >= 4) {
        try {
            int resolution = std::stoi(argv[3]);
            px_height = px_width = resolution;
            std::cout << "Got resolution: " << resolution << "\n";
        } catch (...) {
            std::cerr << "Invalid resolution, using default.\n";
        }
    }

    // Optional - Get gamma
    if (argc >= 5) {
        try {
            gamma = std::stod(argv[4]);
            std::cout << "Got gamma: " << gamma << "\n";
        } catch (...) {
            std::cerr << "Invalid gamma value, using default.\n";
        }
    }

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
    //int aa_samples = scene_parser.get_aa_multisampling();
    camera cam(camera_center, px_height, px_width, color(0, 0, 0)); // black bg
    cam.render(scene, light_sources, ambient, output_file, aa_samples, gamma);

    // Clean up memory
    for (auto* obj : scene) delete obj;
    for (auto* l : light_sources) delete l;

    return 0;
}
