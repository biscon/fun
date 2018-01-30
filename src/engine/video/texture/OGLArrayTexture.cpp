//
// Created by bison on 28-01-2018.
//

#include <SDL_log.h>
#include "OGLArrayTexture.h"

void OGLArrayTexture::init(i32 width, i32 height, i32 layers) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
    this->width = width;
    this->height = height;
    this->layers = layers;
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 5, GL_RGBA8, width, height, layers);
    SDL_Log("Allocating array texture %dx%d with %d layers", width, height, layers);
}

void OGLArrayTexture::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
}

void OGLArrayTexture::bind(int tex_unit) {
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
}

void OGLArrayTexture::uploadLayer(PixelBuffer *pb, i32 layer) {
    SDL_Log("Uploading texture %s at depth %d", pb->getName().c_str(), layer);
    glTexSubImage3D( GL_TEXTURE_2D_ARRAY,
                     0,                       //Mipmap number
                     0,0,layer,                 //xoffset, yoffset, zoffset
                     width,height,1,                 //width, height, depth
                     GL_RGBA,                //format
                     GL_UNSIGNED_BYTE,      //type
                     (void *) pb->getPixels());                //pointer to data
}

void OGLArrayTexture::finalizeUpload() {
    /*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    */
    // anisotropic filtering

    float aniso = 16.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    SDL_Log("Finalizing texture");

}
