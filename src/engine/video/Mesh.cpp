//
// Created by bison on 19-11-2017.
//

#include <cstring>
#include <SDL_log.h>
#include "Mesh.h"

Mesh::Mesh(MeshUpdateType) : type(type) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
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

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Mesh::upload() {
    //SDL_Log("Uploading %d vertices and %d elements", vertices.size(), elements.size());
    GLenum t = GL_STATIC_DRAW;
    if(type == MeshUpdateType::DYNAMIC)
        t = GL_DYNAMIC_DRAW;
    if(type == MeshUpdateType::STREAMING)
        t = GL_STREAM_DRAW;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], t);
}

void Mesh::generateTexturedCube()
{
    vertices.insert(vertices.end(), {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    });
}

/*
void Mesh::addTexturedQuad(float left, float top, float right, float bottom, UVRect& uvRect)
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
*/

void Mesh::clear() {
    vertices.clear();
}

void Mesh::bindVAO() {
    //SDL_Log("Binding vertex array %d", vao);
    glBindVertexArray(vao);
}

void Mesh::bindVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

