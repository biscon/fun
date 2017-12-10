//
// Created by bison on 19-11-2017.
//

#ifndef GAME_QUADBUFFER_H
#define GAME_QUADBUFFER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "TextureAtlas.h"

class RenderMesh {
public:
    RenderMesh();
    virtual ~RenderMesh();
    void upload();
    void bindVAO();
    void bindVBO();
    void clear();
    void addTexturedQuad(float left, float top, float right, float bottom, UVRect& uvRect);

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    std::vector<float> vertices;
    std::vector<GLuint> elements;

private:
};


#endif //GAME_QUADBUFFER_H
