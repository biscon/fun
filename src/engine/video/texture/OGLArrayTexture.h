//
// Created by bison on 28-01-2018.
//

#ifndef GAME_OGLARRAYTEXTURE_H
#define GAME_OGLARRAYTEXTURE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "ITexture.h"
#include "engine/video/raster/PixelBuffer.h"

class OGLArrayTexture {
public:
    void init(i32 width, i32 height, i32 layers);
    void uploadLayer(PixelBuffer *pb, i32 layer);
    void finalizeUpload();

    void bind();
    void bind(int tex_unit);

    GLuint tex;
    i32 width;
    i32 height;
    i32 layers;
private:
};


#endif //GAME_OGLARRAYTEXTURE_H
