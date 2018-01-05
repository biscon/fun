//
// Created by bison on 19-11-2017.
//

#ifndef GAME_OGLTEXTURE_H
#define GAME_OGLTEXTURE_H

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include "ITexture.h"
#include "engine/video/raster/PixelBuffer.h"

class OGLTexture : public ITexture {
public:
    explicit OGLTexture(PixelBuffer* pb, std::string type);
    explicit OGLTexture(PixelBuffer* pb, std::string type, bool filtering);
    virtual ~OGLTexture();

    void bind();
    void bind(int tex_unit);

    u32 getWidth() override;
    u32 getHeight() override;

    GLuint tex;
    u32 width;
    u32 height;
    std::string type;

private:
    bool didInit = false;
    bool init(PixelBuffer *pb, bool filtering);
};


#endif //GAME_OGLTEXTURE_H
