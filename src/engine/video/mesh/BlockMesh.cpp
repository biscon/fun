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
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, vsize, (void *) 4);
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

void BlockMesh::clear() {
    vertices.clear();
    indices.clear();
}


void BlockMesh::generateTexturedCubeAt(i8 x, i8 y, i8 z, BlockFaces& faces, FaceLight &faceLight) {
    if(faceLight.back == 0)
        faceLight.back = 1;
    if(faceLight.front == 0)
        faceLight.front = 1;
    if(faceLight.left == 0)
        faceLight.left = 1;
    if(faceLight.right == 0)
        faceLight.right = 1;
    if(faceLight.bottom == 0)
        faceLight.bottom = 1;
    if(faceLight.top == 0)
        faceLight.top = 1;
    
    u8 ba_l = (u8)(((float)faceLight.back / 15.0f) * 255.0f);
    u8 fr_l = (u8)(((float)faceLight.front / 15.0f) * 255.0f);
    u8 le_l = (u8)(((float)faceLight.left / 15.0f) * 255.0f);
    u8 ri_l = (u8)(((float)faceLight.right / 15.0f) * 255.0f);
    u8 bo_l = (u8)(((float)faceLight.bottom / 15.0f) * 255.0f);
    u8 to_l = (u8)(((float)faceLight.top / 15.0f) * 255.0f);

    /*
    if(level == 1)
    {
        SDL_Log("Converted lightlevel %d to rgb intensity %d", level, l);
    }
    */
    // first 3 vertices is right triangle
    if(faces.back)
        vertices.insert(vertices.end(),{
                // Back face (The North Face ;)
                {byte4(x,y,z,0), ubyte4(ba_l,ba_l,ba_l,0)},
                {byte4(x+1,y+1,z,0), ubyte4(ba_l,ba_l,ba_l,3)},
                {byte4(x+1,y,z,0), ubyte4(ba_l,ba_l,ba_l,2)},
                {byte4(x+1,y+1,z,0), ubyte4(ba_l,ba_l,ba_l,3)},
                {byte4(x,y,z,0), ubyte4(ba_l,ba_l,ba_l,0)},
                {byte4(x,y+1,z,0), ubyte4(ba_l,ba_l,ba_l,1)}
        });
    if(faces.front)
        vertices.insert(vertices.end(),{
                // Front face
                {byte4(x,y,z+1,1), ubyte4(fr_l,fr_l,fr_l,0)},
                {byte4(x+1,y,z+1,1), ubyte4(fr_l,fr_l,fr_l,2)},
                {byte4(x+1,y+1,z+1,1), ubyte4(fr_l,fr_l,fr_l,3)},
                {byte4(x+1,y+1,z+1,1), ubyte4(fr_l,fr_l,fr_l,3)},
                {byte4(x,y+1,z+1,1), ubyte4(fr_l,fr_l,fr_l,1)},
                {byte4(x,y,z+1,1), ubyte4(fr_l,fr_l,fr_l,0)}
        });
    if(faces.left)
        vertices.insert(vertices.end(),{
                // Left face
                {byte4(x,y+1,z+1,2), ubyte4(le_l,le_l,le_l,3)},
                {byte4(x,y+1,z,2), ubyte4(le_l,le_l,le_l,1)},
                {byte4(x,y,z,2), ubyte4(le_l,le_l,le_l,0)},
                {byte4(x,y,z,2), ubyte4(le_l,le_l,le_l,0)},
                {byte4(x,y,z+1,2), ubyte4(le_l,le_l,le_l,2)},
                {byte4(x,y+1,z+1,2), ubyte4(le_l,le_l,le_l,3)}
        });
    if(faces.right)
        vertices.insert(vertices.end(),{
                // Right face
                {byte4(x+1,y+1,z+1,3), ubyte4(ri_l,ri_l,ri_l,1)},
                {byte4(x+1,y,z,3), ubyte4(ri_l,ri_l,ri_l,2)},
                {byte4(x+1,y+1,z,3), ubyte4(ri_l,ri_l,ri_l,3)},
                {byte4(x+1,y,z,3), ubyte4(ri_l,ri_l,ri_l,2)},
                {byte4(x+1,y+1,z+1,3), ubyte4(ri_l,ri_l,ri_l,1)},
                {byte4(x+1,y,z+1,3), ubyte4(ri_l,ri_l,ri_l,0)}
        });
    if(faces.bottom)
        vertices.insert(vertices.end(),{
                // Bottom face
                {byte4(x,y,z,4), ubyte4(bo_l,bo_l,bo_l,3)},
                {byte4(x+1,y,z,4), ubyte4(bo_l,bo_l,bo_l,1)},
                {byte4(x+1,y,z+1,4), ubyte4(bo_l,bo_l,bo_l,0)},
                {byte4(x+1,y,z+1,4), ubyte4(bo_l,bo_l,bo_l,0)},
                {byte4(x,y,z+1,4), ubyte4(bo_l,bo_l,bo_l,2)},
                {byte4(x,y,z,4), ubyte4(bo_l,bo_l,bo_l,3)}
        });
    if(faces.top)
        vertices.insert(vertices.end(),{
                // Top face
                {byte4(x,y+1,z,5), ubyte4(to_l,to_l,to_l,1)},
                {byte4(x+1,y+1,z+1,5), ubyte4(to_l,to_l,to_l,2)},
                {byte4(x+1,y+1,z,5), ubyte4(to_l,to_l,to_l,3)},
                {byte4(x+1,y+1,z+1,5), ubyte4(to_l,to_l,to_l,2)},
                {byte4(x,y+1,z,5), ubyte4(to_l,to_l,to_l,1)},
                {byte4(x,y+1,z+1,5), ubyte4(to_l,to_l,to_l,0)}
        });
}


