//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H
#include <glm.hpp>
#include <memory>
#include "engine/video/texture/OGLTexture.h"
#include "Shader.h"

struct Material {
    std::string name = "defaultMaterial";
    glm::vec4 ambient = {0.2f, 0.2f, 0.2f, 1.0f};
    glm::vec4 diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
    glm::vec4 specular = {1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<OGLTexture> diffuseTexture;
    std::shared_ptr<OGLTexture> specularTexture;
    std::string diffuseMap;
    std::string specularMap;
    float shininess = 32.0f;

    void applyShader(const Shader& shader)
    {
        shader.setVec4("material.ambient", ambient);
        shader.setVec4("material.diffuse", diffuse);
        shader.setVec4("material.specular", specular);
        shader.setFloat("material.shininess", shininess);
        if(diffuseTexture != nullptr)
        {
            shader.setInt("material.hasTexture", 1);
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("texture_diffuse", 0);
            glBindTexture(GL_TEXTURE_2D, diffuseTexture->tex);
        }
        if(specularTexture != nullptr)
        {
            glActiveTexture(GL_TEXTURE1);
            shader.setInt("texture_specular", 1);
            glBindTexture(GL_TEXTURE_2D, specularTexture->tex);
        }
    }
};


#endif //GAME_MATERIAL_H
