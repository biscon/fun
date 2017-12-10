//
// Created by bison on 19-11-2017.
//

#ifndef GAME_OGLTEXTURE_H
#define GAME_OGLTEXTURE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "ITexture.h"
#include "PixelBuffer.h"

class OGLTexture : public ITexture {
public:
    explicit OGLTexture(PixelBuffer* pb);
    virtual ~OGLTexture();

    void bind();

    uint32_t getWidth() override;
    uint32_t getHeight() override;

    GLuint tex;
    uint32_t width;
    uint32_t height;

private:
    bool didInit = false;
    bool init(PixelBuffer *pb);
};


#endif //GAME_OGLTEXTURE_H
