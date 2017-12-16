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
    explicit OGLTexture(PixelBuffer* pb, bool filtering);
    virtual ~OGLTexture();

    void bind();
    void bind(int tex_unit);

    uint32_t getWidth() override;
    uint32_t getHeight() override;

    GLuint tex;
    uint32_t width;
    uint32_t height;

private:
    bool didInit = false;
    bool init(PixelBuffer *pb, bool filtering);
};


#endif //GAME_OGLTEXTURE_H
