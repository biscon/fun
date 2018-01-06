//
// Created by bison on 16-12-2017.
//

#include "BlockMesh.h"

BlockMesh::BlockMesh() {
    init();
}

BlockMesh::~BlockMesh() {
    if(VBO > 0)
        glDeleteBuffers(1, &VBO);
    if(EBO > 0)
        glDeleteBuffers(1, &EBO);
    if(VAO > 0)
        glDeleteVertexArrays(1, &VAO);

}

void BlockMesh::init() {
    //vertices.reserve(256000);
}

void BlockMesh::prepare()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    upload();

    auto vsize = static_cast<GLsizei>(vertexSize);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_BYTE, GL_FALSE, vsize, (void*)0);
    // vertex colors
    if(hasColors) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_BYTE, GL_FALSE, vsize, (void *) 4);
    }
    // vertex texture coords
    if(hasTexcoords) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_SHORT, GL_FALSE, vsize, (void *) 8);
    }
    glBindVertexArray(0);
}

void BlockMesh::upload()
{
    GLenum t = GL_STATIC_DRAW;
    if(type == MeshUpdateType::DYNAMIC)
        t = GL_DYNAMIC_DRAW;
    if(type == MeshUpdateType::STREAMING)
        t = GL_STREAM_DRAW;

    if(!vertices.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BlockMeshVertex), &vertices[0], t);
    }

    if(!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], t);
    }
}

void BlockMesh::drawRange(const Shader& shader, i32 start, i32 count, Material* material)
{
    if(material != nullptr) {
        material->applyShader(shader);
        glActiveTexture(GL_TEXTURE0);
    }

    glBindVertexArray(VAO);
    // draw mesh
    glDrawArrays(GL_TRIANGLES, start, count);
    //glBindVertexArray(0);
}

/*
void BlockMesh::generateCubeAt(float x, float y, float z) {
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
*/

void BlockMesh::clear() {
    vertices.clear();
    indices.clear();
}


void BlockMesh::generateTexturedCubeAt(i8 x, i8 y, i8 z) {
   /*
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
    */

    /*
     * uvRect.left = 0.0f;
    uvRect.right = 1.0f;
    uvRect.top = 1.0f;
    uvRect.bottom = 0.0f;
     */
    vertices.insert(vertices.end(),{
            // Back face (The North Face ;)
            {byte4(x,y,z,0), byte4(0,0,-1,0), short2(0, 0)},
            {byte4(x+1,y+1,z,0), byte4(0,0,-1,0), short2(1, 1)},
            {byte4(x+1,y,z,0), byte4(0,0,-1,0), short2(1, 0)},
            {byte4(x+1,y+1,z,0), byte4(0,0,-1,0), short2(1, 1)},
            {byte4(x,y,z,0), byte4(0,0,-1,0), short2(0, 0)},
            {byte4(x,y+1,z,0), byte4(0,0,-1,0), short2(0, 1)},
            // Front face
            {byte4(x,y,z+1,0), byte4(0,0,1,0), short2(0, 0)},
            {byte4(x+1,y,z+1,0), byte4(0,0,1,0), short2(1, 0)},
            {byte4(x+1,y+1,z+1,0), byte4(0,0,1,0), short2(1, 1)},
            {byte4(x+1,y+1,z+1,0), byte4(0,0,1,0), short2(1, 1)},
            {byte4(x,y+1,z+1,0), byte4(0,0,1,0), short2(0, 1)},
            {byte4(x,y,z+1,0), byte4(0,0,1,0), short2(0, 0)},
            // Left face
            {byte4(x,y+1,z+1,0), byte4(-1,0,0,0), short2(1, 1)},
            {byte4(x,y+1,z,0), byte4(-1,0,0,0), short2(0, 1)},
            {byte4(x,y,z,0), byte4(-1,0,0,0), short2(0, 0)},
            {byte4(x,y,z,0), byte4(-1,0,0,0), short2(0, 0)},
            {byte4(x,y,z+1,0), byte4(-1,0,0,0), short2(1, 0)},
            {byte4(x,y+1,z+1,0), byte4(-1,0,0,0), short2(1, 1)},
            // Right face
            {byte4(x+1,y+1,z+1,0), byte4(1,0,0,0), short2(0, 1)},
            {byte4(x+1,y,z,0), byte4(1,0,0,0), short2(1, 0)},
            {byte4(x+1,y+1,z,0), byte4(1,0,0,0), short2(1, 1)},
            {byte4(x+1,y,z,0), byte4(1,0,0,0), short2(1, 0)},
            {byte4(x+1,y+1,z+1,0), byte4(1,0,0,0), short2(0, 1)},
            {byte4(x+1,y,z+1,0), byte4(1,0,0,0), short2(0, 0)},
            // Bottom face
            {byte4(x,y,z,0), byte4(0,-1,0,0), short2(1, 1)},
            {byte4(x+1,y,z,0), byte4(0,-1,0,0), short2(0, 1)},
            {byte4(x+1,y,z+1,0), byte4(0,-1,0,0), short2(0, 0)},
            {byte4(x+1,y,z+1,0), byte4(0,-1,0,0), short2(0, 0)},
            {byte4(x,y,z+1,0), byte4(0,-1,0,0), short2(1, 0)},
            {byte4(x,y,z,0), byte4(0,-1,0,0), short2(1, 1)},
            // Top face
            {byte4(x,y+1,z,0), byte4(0,1,0,0), short2(0, 1)},
            {byte4(x+1,y+1,z+1,0), byte4(0,1,0,0), short2(1, 0)},
            {byte4(x+1,y+1,z,0), byte4(0,1,0,0), short2(1, 1)},
            {byte4(x+1,y+1,z+1,0), byte4(0,1,0,0), short2(1, 0)},
            {byte4(x,y+1,z,0), byte4(0,1,0,0), short2(0, 1)},
            {byte4(x,y+1,z+1,0), byte4(0,1,0,0), short2(0, 0)}
    });
}


