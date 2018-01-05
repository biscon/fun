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
    /*
    cube.insert(cube.end(),{
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            +0.5f, +0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // top-right
            +0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-right
            +0.5f, +0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // top-right
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, +0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // top-left
            // Front face
            -0.5f, -0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            +0.5f, -0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
            +0.5f, +0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-right
            +0.5f, +0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-right
            -0.5f, +0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-left
            -0.5f, -0.5f, +0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // Left face
            -0.5f, +0.5f, +0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
            -0.5f, +0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, +0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, +0.5f, +0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
            // Right face
            +0.5f, +0.5f, +0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-left
            +0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            +0.5f, +0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
            +0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            +0.5f, +0.5f, +0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-left
            +0.5f, -0.5f, +0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // top-right
            +0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // top-left
            +0.5f, -0.5f, +0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            +0.5f, -0.5f, +0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, +0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // top-right
            // Top face
            -0.5f, +0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // top-left
            +0.5f, +0.5f, +0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            +0.5f, +0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // top-right
            +0.5f, +0.5f, +0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, +0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // top-left
            -0.5f, +0.5f, +0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    });
     */
}

/*
void Mesh3::generateTexturedCubeAt(float x, float y, float z, UVRect &r) {
    vertices.insert(vertices.end(), cube.begin(), cube.end());
}
 */

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
    if(material != nullptr) {
       material->applyShader(shader);
    }

    // draw mesh
    glBindVertexArray(VAO);
    if(!indices.empty())
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / vertexSize);
    glBindVertexArray(0);
}

void Mesh3::drawRange(const Shader& shader, int32_t start, int32_t count, Material* material)
{
    if(material != nullptr) {
        material->applyShader(shader);
        glActiveTexture(GL_TEXTURE0);
    }

    glBindVertexArray(VAO);
    // draw mesh
    glDrawArrays(GL_TRIANGLES, start / vertexSize, count / vertexSize);
    //glBindVertexArray(0);
}

void Mesh3::generateCubeAt(float x, float y, float z) {
    /*
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
    */

    vertices.insert(vertices.end(),{
            // Back face
            x-0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // Bottom-left
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // top-right
            x+0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // top-right
            x-0.5f, y-0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // bottom-left
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  0.0f, -1.0f, // top-left
            // Front face
            x-0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // bottom-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // bottom-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // top-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // top-right
            x-0.5f, y+0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // top-left
            x-0.5f, y-0.5f, z+0.5f, 0.0f,  0.0f, 1.0f, // bottom-left
            // Left face
            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, // top-right
            x-0.5f, y+0.5f, z-0.5f, -1.0f,  0.0f,  0.0f,  // top-left
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f,  // bottom-left
            x-0.5f, y-0.5f, z-0.5f, -1.0f,  0.0f,  0.0f, // bottom-left
            x-0.5f, y-0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, // bottom-right
            x-0.5f, y+0.5f, z+0.5f, -1.0f,  0.0f,  0.0f, // top-right
            // Right face
            x+0.5f, y+0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, // top-left
            x+0.5f, y-0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, // top-right
            x+0.5f, y-0.5f, z-0.5f, 1.0f,  0.0f,  0.0f, // bottom-right
            x+0.5f, y+0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, // top-left
            x+0.5f, y-0.5f, z+0.5f, 1.0f,  0.0f,  0.0f, // bottom-left
            // Bottom face
            x-0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, // top-right
            x+0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, // top-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, // bottom-left
            x+0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, // bottom-left
            x-0.5f, y-0.5f, z+0.5f, 0.0f, -1.0f,  0.0f, // bottom-right
            x-0.5f, y-0.5f, z-0.5f, 0.0f, -1.0f,  0.0f, // top-right
            // Top face
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, // top-left
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, // bottom-right
            x+0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, // top-right
            x+0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, // bottom-right
            x-0.5f, y+0.5f, z-0.5f, 0.0f,  1.0f,  0.0f, // top-left
            x-0.5f, y+0.5f, z+0.5f, 0.0f,  1.0f,  0.0f, // bottom-left
    });
}

void Mesh3::clear() {
    vertices.clear();
    indices.clear();
}


void Mesh3::generateTexturedCubeAt(float x, float y, float z, UVRect &r) {
    /*
    r.left = 0.0f;
    r.right = 1.0f;
    r.bottom = 0.0f;
    r.top = 1.0f;
     */
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

/*
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
 */

Mesh3::~Mesh3() {
    if(VBO > 0)
        glDeleteBuffers(1, &VBO);
    if(EBO > 0)
        glDeleteBuffers(1, &EBO);
    if(VAO > 0)
        glDeleteVertexArrays(1, &VAO);

}

