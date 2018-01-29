//
// Created by bison on 16-12-2017.
//

#ifndef GAME_FACEMATERIAL_H
#define GAME_FACEMATERIAL_H
#include <glm.hpp>
#include <memory>
#include <engine/video/texture/OGLArrayTexture.h>
#include "Shader.h"

struct FaceMaterial {
    glm::vec4 color = {0.8f, 0.8f, 0.8f, 1.0f};
    //OGLArrayTexture* texture = nullptr;
    std::string textureMap;
    i32 layer;
    std::shared_ptr<PixelBuffer> buffer;
};


#endif //GAME_FACEMATERIAL_H
