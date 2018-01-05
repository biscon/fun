//
// Created by bison on 02-01-2018.
//

#ifndef GAME_DIRECTIONALLIGHT_H
#define GAME_DIRECTIONALLIGHT_H

#include <glm/glm.hpp>
#include "Shader.h"

class DirectionalLight {
public:
    glm::vec3 color;
    glm::vec3 direction;
    float intensity;
    bool active;

    DirectionalLight() {
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        direction = glm::vec3(0.0f, -1.0f, 0.0f);
        intensity = 0.9f;
        active = true;
    }

    DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction, float intensity, bool active) :
            color(color), direction(direction),intensity(intensity),active(active) {}

    void applyShader(const Shader& shader)
    {
        shader.setVec3("dirLight.color", color);
        shader.setVec3("dirLight.direction", direction);
        shader.setInt("dirLight.is_active", (int) active);
        shader.setFloat("dirLight.intensity", intensity);
    }
};


#endif //GAME_DIRECTIONALLIGHT_H
