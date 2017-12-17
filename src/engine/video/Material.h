//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H
#include <glm.hpp>
#include <memory>
#include "OGLTexture.h"

struct Material {
    std::string name = "defaultMaterial";
    glm::vec3 ambient = {0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse = {0.8f, 0.8f, 0.8f};
    glm::vec3 specular = {1.0f, 1.0f, 1.0f};
    std::shared_ptr<OGLTexture> diffuseTexture;
    std::shared_ptr<OGLTexture> specularTexture;
    std::string diffuseMap;
    std::string specularMap;
    float shininess = 32.0f;
};


#endif //GAME_MATERIAL_H
