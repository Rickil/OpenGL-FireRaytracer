//
// Created by yanis on 29/06/2023.
//

#ifndef POGL_MATERIAL_HH
#define POGL_MATERIAL_HH

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "TGA.hh"
#include "Vector3.hh"

class Material {
public:
    std::string name;

    Tga* kdMap = nullptr;
    Tga* ksMap = nullptr;
    Vector3 kd;
    Vector3 ks;

    Material(){}
    ~Material(){
        delete kdMap;
        delete ksMap;
    }

    static std::vector<Material*> loadMaterials(const char* path){

        std::string newPath = path;  // Convert the const char* to std::string

        // Find the position of the file extension
        size_t extensionPos = newPath.find_last_of(".");
        if (extensionPos != std::string::npos) {
            // Replace the file extension with ".mtl"
            newPath.replace(extensionPos, newPath.length() - extensionPos, ".mtl");
        } else {
            // Append ".mtl" to the end of the path
            newPath += ".mtl";
        }

        // Convert the std::string back to const char* if necessary
        const char* newFilePath = newPath.c_str();

        std::vector<Material*> materials;
        std::ifstream file(newFilePath);
        if (!file) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return materials;
        }

        Material* currentMaterial = nullptr;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string command;
            if (!(iss >> command))
                continue;

            if (command == "newmtl") {
                std::string materialName;
                if (iss >> materialName) {
                    currentMaterial = new Material();
                    currentMaterial->name = materialName;
                    materials.push_back(currentMaterial);
                }
            }
            else if (command == "map_Kd") {
                std::string filePath;
                if (iss >> filePath) {
                    currentMaterial->kdMap = new Tga(filePath.c_str());
                }
            }
            /*else if (command == "map_Ks") {
                std::string filePath;
                if (iss >> filePath) {
                    currentMaterial->ksMap = new Tga(filePath.c_str());
                }
            }*/
            else if (command == "Kd") {
                float r, g, b;
                if (iss >> r >> g >> b) {
                    currentMaterial->kd = Vector3(r,g,b);
                }
            }
            else if (command == "Ks") {
                float r, g, b;
                if (iss >> r >> g >> b) {
                    currentMaterial->ks = Vector3(r,g,b);
                }
            }
        }

        file.close();

        //load textures


        // Iterate over each Tga image and upload its pixel data to the texture array.
        /*for (size_t i = 0; i < materials.size(); i++)
        {
            Tga* tgaImage = materials[i]->kdMap;

            if (tgaImage) {
                // Upload pixel data for the current image to the corresponding layer.
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tgaImage->GetWidth(), tgaImage->GetHeight(), 1,
                                GL_BGRA, GL_UNSIGNED_BYTE, tgaImage->GetPixels().data());
            }
        }*/

        return materials;
    }
};


#endif //POGL_MATERIAL_HH
