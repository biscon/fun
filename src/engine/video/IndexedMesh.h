//
// Created by bison on 19-11-2017.
//

#ifndef GAME_INDEXEDMESH_H
#define GAME_INDEXEDMESH_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "TextureAtlas.h"
#include "MeshUpdateType.h"


class IndexedMesh {
public:
    IndexedMesh(MeshUpdateType updateType);
    virtual ~IndexedMesh();
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
    MeshUpdateType type;

private:
};


#endif //GAME_QUADBUFFER_H
