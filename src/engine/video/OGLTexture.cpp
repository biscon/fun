//
// Created by bison on 19-11-2017.
//

#include <SDL_log.h>
#include "OGLTexture.h"

OGLTexture::OGLTexture(PixelBuffer *pb, std::string type) : type(type) {
    init(pb, false);
}

OGLTexture::OGLTexture(PixelBuffer *pb, std::string type, bool filtering) : type(type) {
    init(pb, filtering);
}

OGLTexture::~OGLTexture() {
    if(didInit)
        glDeleteTextures(1, &tex);
}

bool OGLTexture::init(PixelBuffer *pb, bool filtering) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    width = pb->getWidth();
    height = pb->getHeight();

    //SDL_Log("glTexImage2D width = %d, height = %d, data = %d", width, height, (void *) pb->getPixels());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) pb->getPixels());
    // set wrap repeat

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     */
    // nearest neighbour filtering
    if(!filtering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        // anisotropic filtering
        float aniso = 16.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // Black/white checkerboard
    /*
    float pixels[] = {
            1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    */
    /*
    uint8_t pixels[] = {
            255, 0, 0, 255,  255, 255, 255, 255,
            255, 255, 255, 255,   255, 0, 0, 255
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
     */

    //glGenerateMipmap(GL_TEXTURE_2D);
    // check OpenGL error
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        //SDL_Log("OpenGL error: %s", err);
    }
    didInit = true;
    return true;
}

u32 OGLTexture::getWidth() {
    return width;
}

u32 OGLTexture::getHeight() {
    return height;
}

void OGLTexture::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void OGLTexture::bind(int tex_unit) {
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, tex);
}

