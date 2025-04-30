#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "vec3.h"

class parser{
    public:
        parser() {}

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
        
    private:
        std::vector<std::string> lines;
};

#endif