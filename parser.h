#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "vec3.h"
#include "sphere.h"
#include "plane.h"
#include "primitive.h"
#include "color.h"
#include "light_source.h"
#include "spotlight.h"
#include "directional_light.h"
#include "definitions.h"

struct scene_object {
  primitive*   shape;
  material_t   material;
};

class parser{
    public:
        parser() {}

        //std::vector<primitive*> get_objects();

        void load(const std::string& filename){
            std::ifstream scene_file(filename);
            if(!scene_file){
                throw std::runtime_error("Failed to open scene file: " + filename);
            }

            lines.clear();
            std::string line;
            while(std::getline(scene_file, line)){
                if (!line.empty()) lines.push_back(line);
            }
        }

        point3 get_eye(){
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string identifier;
                iss >> identifier;

                if(identifier == "e"){
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
                    return point3(x, y ,z);
                }
            }
            throw std::runtime_error("No 'e' (eye) line found in scene file.");
        }

        int get_aa_samples(){
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string identifier;
                iss >> identifier;

                if(identifier == "e"){
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
                    int samples = int(w);
                    if(samples < 1) samples = 1;
                    return samples;
                }
            }
            throw std::runtime_error("No 'e' (eye) line found in scene file.");
        }        

        // Get scene objects
        std::vector<scene_object> get_scene_objects(){
            std::vector<scene_object> result;
            std::vector<material_t> materials;
        
            // First collect materials
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string id;
                iss >> id;
        
                if(id == "c"){
                    double r, g, b, shininess;
                    iss >> r >> g >> b >> shininess;
                    material_t mat;
                    mat.ambient = color(r, g, b);
                    mat.diffuse = color(r, g, b);
                    mat.shininess = shininess;
                    materials.push_back(mat);
                }
            }
        
            size_t mat_index = 0;
        
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string id;
                iss >> id;
        
                if(id == "o" || id == "t" || id == "r"){
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
        
                    primitive* obj = nullptr;
                    if(w > 0){ // Sphere
                        obj = new sphere(point3(x, y, z), w);
                    } else { // Plane
                        obj = new plane(x, y, z, w);
                    }
        
                    if(!obj) continue;
        
                    material_t mat = mat_index < materials.size() ? materials[mat_index] : material_t{};
                    result.push_back({obj, mat});
                    mat_index++;
                }
            }
        
            return result;
        }

        // Get ambient color
        color get_ambient(){
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string identifier;
                iss >> identifier;
                if(identifier == "a"){
                    double r, g, b, _;
                    iss >> r >> g >> b >> _;
                    return color(r, g, b);
                }
            }
            throw std::runtime_error("Ambient light not found.");
        }

        std::vector<light_source*> get_lights() {
            std::vector<std::pair<vec3, double>> rawDirs;   // pair of direction and w (0=dir, 1=spot)
            std::vector<point3> positions;
            std::vector<double> cutoffs;
            std::vector<color> intensities;

            for (const auto& line : lines) {
                std::istringstream iss(line);
                std::string id;
                iss >> id;

                if (id == "d") {
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
                    rawDirs.push_back(std::make_pair(vec3(x, y, z), w));
                }
                else if (id == "p") {
                    double x, y, z, cutoffCos;
                    iss >> x >> y >> z >> cutoffCos;
                    positions.push_back(point3(x, y, z));
                    cutoffs.push_back(cutoffCos);
                }
                else if (id == "i") {
                    double r, g, b, _;
                    iss >> r >> g >> b >> _;
                    intensities.push_back(color(r, g, b));
                }
            }

            std::vector<light_source*> result;
            size_t spotIndex = 0;

            for (size_t idx = 0; idx < rawDirs.size(); idx++) {
                const std::pair<vec3, double>& dirPair = rawDirs[idx];
                const vec3& dirVec = dirPair.first;
                double w = dirPair.second;
                color col = (idx < intensities.size() ? intensities[idx] : color(1, 1, 1));

                if (w == 0.0) {
                    // Directional light: w == 0
                    result.push_back(new directional_light(dirVec, col));
                } else {
                    // Spotlight: w == 1
                    if (spotIndex < positions.size() && spotIndex < cutoffs.size()) {
                        result.push_back(
                            new spotlight(
                                positions[spotIndex],   
                                dirVec,                 
                                cutoffs[spotIndex],     
                                col                     
                            )
                        );
                        spotIndex++;
                    }
                }
            }
            
            return result;
        } 
        
    private:
        std::vector<std::string> lines;
};

#endif