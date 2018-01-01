//
// Created by bison on 01-01-2018.
//

#ifndef GAME_FOG_H
#define GAME_FOG_H

#include <glm/glm.hpp>
#include "Shader.h"

class Fog {
    glm::vec3 color;
    float density;
    bool active;

public:
    Fog() {
        color = glm::vec3(0,0,0);
        active = false;
        density = 0;
    }

    Fog(const glm::vec3 &color, float density, bool active) : color(color), density(density), active(active) {}

    void applyShader(const Shader& shader)
    {
        shader.setVec3("fog.color", color);
        shader.setInt("fog.is_active", (int) active);
        shader.setFloat("fog.density", density);
    }
};


#endif //GAME_FOG_H
