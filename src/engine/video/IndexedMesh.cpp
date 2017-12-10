//
// Created by bison on 19-11-2017.
//

#include <cstring>
#include <SDL_log.h>
#include "IndexedMesh.h"

IndexedMesh::IndexedMesh(MeshUpdateType) : type(type) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    /*
     float vertices[] = {
        //  Position      Color             Texcoords
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
            -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
        };

    vertices = {
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,       // Top-left
            640.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     // Top-right
            640.0f, 360.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // Bottom-right
            0.0f, 360.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f      // Bottom-left
    };
    elements = {
            0, 1, 2,
            2, 3, 0
    };
     */
}

IndexedMesh::~IndexedMesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void IndexedMesh::upload() {
    //SDL_Log("Uploading %d vertices and %d elements", vertices.size(), elements.size());
    GLenum t = GL_STATIC_DRAW;
    if(type == MeshUpdateType::DYNAMIC)
        t = GL_DYNAMIC_DRAW;
    if(type == MeshUpdateType::STREAMING)
        t = GL_STREAM_DRAW;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], t);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &elements[0], t);
}

void IndexedMesh::addTexturedQuad(float left, float top, float right, float bottom, UVRect& uvRect)
{
    vertices.insert(vertices.end(), {
            left, top, 1.0f, 1.0f, 1.0f, 1.0f, uvRect.left, uvRect.top,
            right, top, 1.0f, 1.0f, 1.0f, 1.0f, uvRect.right, uvRect.top,
            right, bottom, 1.0f, 1.0f, 1.0f, 1.0f, uvRect.right, uvRect.bottom,
            left, bottom, 1.0f, 1.0f, 1.0f, 1.0f, uvRect.left, uvRect.bottom
    });
    GLuint off = (vertices.size()/8) - 4;
    elements.insert(elements.end(), {
            off, off + 1, off + 2,
            off + 2, off + 3, off
    });
}

void IndexedMesh::clear() {
    vertices.clear();
    elements.clear();
}

void IndexedMesh::bindVAO() {
    //SDL_Log("Binding vertex array %d", vao);
    glBindVertexArray(vao);
}

void IndexedMesh::bindVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

