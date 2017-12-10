//
// Created by bison on 19-11-2017.
//

#ifndef GAME_MESH_H
#define GAME_MESH_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "TextureAtlas.h"
#include "MeshUpdateType.h"


class Mesh {
public:
    Mesh(MeshUpdateType updateType);
    virtual ~Mesh();
    void upload();
    void bindVAO();
    void bindVBO();
    void clear();
    void generateTexturedCube();

    GLuint vao;
    GLuint vbo;
    std::vector<float> vertices;
    MeshUpdateType type;

private:
};


#endif //GAME_QUADBUFFER_H
