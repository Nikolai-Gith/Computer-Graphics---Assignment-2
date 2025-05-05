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
#include "definitions.h"

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
        
                if(id == "o"){
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

        // Get Directional Lights
        std::vector<light_t> get_directional_lights(){
            std::vector<light_t> result;
        
            for (size_t i = 0; i < lines.size(); ++i){
                std::istringstream iss(lines[i]);
                std::string identifier;
                iss >> identifier;
        
                if(identifier == "d"){
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
                    if(w == 0.0){
                        // Directional light
                        vec3 dir(x, y, z);
        
                        // Look ahead for corresponding intensity
                        if (i+1 < lines.size()){
                            std::istringstream iss2(lines[i+1]);
                            std::string next;
                            iss2 >> next;
                            if(next == "i"){
                                double r, g, b, _;
                                iss2 >> r >> g >> b >> _;
                                result.push_back({dir, color(r, g, b)});
                            }
                        }
                    }
                }
            }
        
            return result;
        }
        
        // Get spotlights
        std::vector<spotlight_t> get_spotlights(){
            std::vector<spotlight_t> result;
        
            std::vector<vec3> directions;
            std::vector<point3> positions;
            std::vector<float> cutoffs;
            std::vector<color> intensities;
        
            for (const auto& line : lines){
                std::istringstream iss(line);
                std::string identifier;
                iss >> identifier;
        
                if(identifier == "d"){
                    double x, y, z, w;
                    iss >> x >> y >> z >> w;
                    if(w == 1.0){
                        directions.push_back(vec3(x, y, z));
                    }
                } else if(identifier == "p"){
                    double x, y, z, cutoff;
                    iss >> x >> y >> z >> cutoff;
                    positions.push_back(point3(x, y, z));
                    cutoffs.push_back(cutoff);
                } else if(identifier == "i"){
                    double r, g, b, _;
                    iss >> r >> g >> b >> _;
                    intensities.push_back(color(r, g, b));
                }
            }
        
            size_t count = std::min(directions.size(), std::min(positions.size(), intensities.size()));
            for(size_t i = 0; i < count; ++i){
                result.push_back({directions[i], positions[i], cutoffs[i], intensities[i]});
            }
        
            return result;
        }
        
    private:
        std::vector<std::string> lines;
};

#endif