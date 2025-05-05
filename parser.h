#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "vec3.h"
#include "sphere.h"
//#include "plane.h"
#include "primitive.h"

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

        std::vector<primitive*> get_objects(){
            std::vector<primitive*> objects;
        
            for(const auto& line : lines){
                std::istringstream iss(line);
                std::string identifier;
                iss >> identifier;
        
                if(identifier == "o"){
                    double x, y, z, r;
                    iss >> x >> y >> z >> r;
                    if (r <= 0) {
                        std::cout << "Negative sphere radius: " << r << " , inverting..." << std::endl;
                        r = abs(r);
                        //continue;
                    }
                    objects.push_back(new sphere(point3(x, y, z), r));
                }
                else if(identifier == "p"){
                    // ignore for now - uncomment when planes implemented
                    // double a, b, c, d;
                    // iss >> a >> b >> c >> d;
                    // if (d > 0) {
                    //     std::cerr << "Warning: plane with d > 0 may be facing away from origin.\n";
                    // }
                    // objects.push_back(new plane(a, b, c, d));
                }
            }
        
            return objects;
        }
        
    private:
        std::vector<std::string> lines;
};

#endif