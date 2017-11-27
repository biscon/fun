//
// Created by bison on 21-11-2017.
//

#ifndef GAME_TEXTUREHANDLE_H
#define GAME_TEXTUREHANDLE_H

#include <memory>
#include "TextureAtlas.h"
#include "../../system/video/OGLTexture.h"

enum TextureStatus { CREATED, ADDED, BUILT, UPLOADED};

class Texture {
public:
    int32_t handle = -1;
    TextureAtlas* atlas = nullptr;
    OGLTexture* texture = nullptr;
    int32_t index = 0;
    TextureStatus status = CREATED;
    std::shared_ptr<PixelBuffer> buffer = nullptr;
};


#endif //GAME_TEXTUREHANDLE_H
