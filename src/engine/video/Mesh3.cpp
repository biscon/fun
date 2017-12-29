//
// Created by bison on 16-12-2017.
//

#include "Mesh3.h"

Mesh3::Mesh3() {
    init();
}

Mesh3::Mesh3(std::vector<float> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices) {
    init();
}

Mesh3::Mesh3(std::vector<float> vertices) : vertices(vertices) {
    init();
}

void Mesh3::init() {
    vertices.reserve(256000);
}

void Mesh3::prepare()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if(!indices.empty())
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    upload();

    auto bytes_vertex = vertexSize * sizeof(float);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, bytes_vertex, (void*)0);
    // vertex normals
    if(hasNormals) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (3 * sizeof(float)));
    }
    // vertex texture coords
    if(hasTexcoords) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, bytes_vertex, (void *) (6 * sizeof(float)));
    }
    glBindVertexArray(0);
}

void Mesh3::upload()
{
    GLenum t = GL_STATIC_DRAW;
    if(type == MeshUpdateType::DYNAMIC)
        t = GL_DYNAMIC_DRAW;
    if(type == MeshUpdateType::STREAMING)
        t = GL_STREAM_DRAW;

    if(!vertices.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], t);
    }

    if(!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], t);
    }
}

void Mesh3::draw(const Shader& shader)
{
    // set material uniforms
    if(hasTexcoords) {
        if (material->diffuseTexture != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("material.texture_diffuse1", 0);
            glBindTexture(GL_TEXTURE_2D, material->diffuseTexture->tex);
        }
        if (material->specularTexture != nullptr) {
            glActiveTexture(GL_TEXTURE1);
            shader.setInt("material.texture_specular1", 1);
            glBindTexture(GL_TEXTURE_2D, material->specularTexture->tex);
        }
    }
    else
    {
        shader.setVec3("material.ambient", material->ambient[0], material->ambient[1], material->ambient[2]);
        shader.setVec3("material.diffuse", material->diffuse[0], material->diffuse[1], material->diffuse[2]);
        shader.setVec3("material.specular", material->specular[0], material->specular[1], material->specular[2]);
    }
    glActiveTexture(GL_TEXTURE0);

    shader.setFloat("material.shininess", material->shininess);


    // draw mesh
    glBindVertexArray(VAO);
    if(!indices.empty())
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / vertexSize);
    glBindVertexArray(0);
}

void Mesh3::generateCubeAt(float x, float y, float z) {
    vertices.insert(vertices.end(),{
            x-0.5f, y-0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,
            x+0.5f, y-0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,
            x+0.5f, y+0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,
            x+0.5f, y+0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,
            x-0.5f, y+0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,
            x-0.5f, y-0.5f, z-0.5f,  0.0f,  0.0f, -1.0f,

            x-0.5f, y-0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,
            x+0.5f, y-0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,
            x+0.5f, y+0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,
            x+0.5f, y+0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,
            x-0.5f, y+0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,
            x-0.5f, y-0.5f, z+0.5f,  0.0f,  0.0f, 1.0f,

            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f,
            x-0.5f, y+0.5f, z-0.5f, -1.0f,  0.0f,  0.0f,
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f,
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f,
            x-0.5f, y-0.5f, z+0.5f, -1.0f,  0.0f,  0.0f,
            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f,

            x+0.5f, y+0.5f, z+0.5f,  1.0f,  0.0f,  0.0f,
            x+0.5f, y+0.5f, z-0.5f,  1.0f,  0.0f,  0.0f,
            x+0.5f, y-0.5f, z-0.5f,  1.0f,  0.0f,  0.0f,
            x+0.5f, y-0.5f, z-0.5f,  1.0f,  0.0f,  0.0f,
            x+0.5f, y-0.5f, z+0.5f,  1.0f,  0.0f,  0.0f,
            x+0.5f, y+0.5f, z+0.5f,  1.0f,  0.0f,  0.0f,

            x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f,  0.0f,
            x+0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f,  0.0f,
            x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f,  0.0f,
            x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f,  0.0f,
            x-0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f,  0.0f,
            x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f,  0.0f,

            x-0.5f, y+0.5f, z-0.5f,  0.0f,  1.0f,  0.0f,
            x+0.5f, y+0.5f, z-0.5f,  0.0f,  1.0f,  0.0f,
            x+0.5f, y+0.5f, z+0.5f,  0.0f,  1.0f,  0.0f,
            x+0.5f, y+0.5f, z+0.5f,  0.0f,  1.0f,  0.0f,
            x-0.5f, y+0.5f, z+0.5f,  0.0f,  1.0f,  0.0f,
            x-0.5f, y+0.5f, z-0.5f,  0.0f,  1.0f,  0.0f
    });
}

void Mesh3::clear() {
    vertices.clear();
    indices.clear();
}

void Mesh3::generateTexturedCubeAt(float x, float y, float z, UVRect &r) {
    vertices.insert(vertices.end(),{
            // Back face
            x-0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.left, r.bottom, // Bottom-left
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.right, r.top, // top-right
            x+0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.right, r.bottom, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.right, r.top, // top-right
            x-0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.left, r.bottom, // bottom-left
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, r.left, r.top, // top-left
            // Front face
            x-0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.left, r.bottom, // bottom-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.right, r.bottom, // bottom-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.right, r.top, // top-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.right, r.top, // top-right
            x-0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.left, r.top, // top-left
            x-0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, r.left, r.bottom, // bottom-left
            // Left face
            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, r.right, r.bottom, // top-right
            x-0.5f, y+0.5f, z-0.5f, -1.0f,  0.0f,  0.0f, r.right, r.top, // top-left
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f, r.left, r.top, // bottom-left
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f, r.left, r.top, // bottom-left
            x-0.5f, y-0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, r.left, r.bottom, // bottom-right
            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, r.right, r.bottom, // top-right
            // Right face
            x+0.5f, y+0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, r.right, r.bottom, // top-left
            x+0.5f, y-0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, r.left, r.top, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, r.right, r.top, // top-right
            x+0.5f, y-0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, r.left, r.top, // bottom-right
            x+0.5f, y+0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, r.right, r.bottom, // top-left
            x+0.5f, y-0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, r.left, r.bottom, // bottom-left
            // Bottom face
            x-0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, r.left, r.top, // top-right
            x+0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, r.right, r.top, // top-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, r.right, r.bottom, // bottom-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, r.right, r.bottom, // bottom-left
            x-0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, r.left, r.bottom, // bottom-right
            x-0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, r.left, r.top, // top-right
            // Top face
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, r.left, r.top, // top-left
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, r.right, r.bottom, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, r.right, r.top, // top-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, r.right, r.bottom, // bottom-right
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, r.left, r.top, // top-left
            x-0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, r.left, r.bottom  // bottom-left
    });
}

Mesh3::~Mesh3() {
    if(VBO > 0)
        glDeleteBuffers(1, &VBO);
    if(EBO > 0)
        glDeleteBuffers(1, &EBO);
    if(VAO > 0)
        glDeleteVertexArrays(1, &VAO);

}

