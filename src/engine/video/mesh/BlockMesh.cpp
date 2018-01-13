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


void BlockMesh::generateTexturedCubeAt(i8 x, i8 y, i8 z, BlockFaces& faces, FaceLight &faceLight, AOBlock& aob) {
    u8 min_level = 15;
    if(faceLight.back == 0)
        faceLight.back = min_level;
    if(faceLight.front == 0)
        faceLight.front = min_level;
    if(faceLight.left == 0)
        faceLight.left = min_level;
    if(faceLight.right == 0)
        faceLight.right = min_level;
    if(faceLight.bottom == 0)
        faceLight.bottom = min_level;
    if(faceLight.top == 0)
        faceLight.top = min_level;
    
    u8 ba_l = (u8)(((float)faceLight.back / 15.0f) * 255.0f);
    u8 fr_l = (u8)(((float)faceLight.front / 15.0f) * 255.0f);
    u8 le_l = (u8)(((float)faceLight.left / 15.0f) * 255.0f);
    u8 ri_l = (u8)(((float)faceLight.right / 15.0f) * 255.0f);
    u8 bo_l = (u8)(((float)faceLight.bottom / 15.0f) * 255.0f);
    u8 to_l = (u8)(((float)faceLight.top / 15.0f) * 255.0f);

    u8 v1,v2,v3,v4;
    float minLight = 0.3f;
    /*
    if(level == 1)
    {
        SDL_Log("Converted lightlevel %d to rgb intensity %d", level, l);
    }
    */
    // first 3 vertices is right triangle
    if(faces.back) {
        /*
        v1 = 0;
        v2 = 255/4;
        v3 = 255/2;
        v4 = 255;
         */
        v1 = (u8) (((float) aob.faces[BACK_FACE].vertices[LEFT_TOP].AO / 3.0f) * fr_l);
        v2 = (u8) (((float) aob.faces[BACK_FACE].vertices[LEFT_BTM].AO / 3.0f) * fr_l);
        v3 = (u8) (((float) aob.faces[BACK_FACE].vertices[RIGHT_BTM].AO / 3.0f) * fr_l);
        v4 = (u8) (((float) aob.faces[BACK_FACE].vertices[RIGHT_TOP].AO / 3.0f) * fr_l);
        vertices.insert(vertices.end(), {
                // Back face (The North Face ;)
                {byte4(x, y, z, 0),         ubyte4(v3, v3, v3, 0)},
                {byte4(x + 1, y + 1, z, 0), ubyte4(v1, v1, v1, 3)},
                {byte4(x + 1, y, z, 0),     ubyte4(v2, v2, v2, 2)},

                {byte4(x + 1, y + 1, z, 0), ubyte4(v1, v1, v1, 3)},
                {byte4(x, y, z, 0),         ubyte4(v3, v3, v3, 0)},
                {byte4(x, y + 1, z, 0),     ubyte4(v4, v4, v4, 1)}
        });
    }
    if(faces.front)
    {
        v1 = (u8) (((float) aob.faces[FRONT_FACE].vertices[LEFT_TOP].AO / 3.0f) * fr_l);
        v2 = (u8) (((float) aob.faces[FRONT_FACE].vertices[LEFT_BTM].AO / 3.0f) * fr_l);
        v3 = (u8) (((float) aob.faces[FRONT_FACE].vertices[RIGHT_BTM].AO / 3.0f) * fr_l);
        v4 = (u8) (((float) aob.faces[FRONT_FACE].vertices[RIGHT_TOP].AO / 3.0f) * fr_l);

        vertices.insert(vertices.end(), {
                // Front face
                {byte4(x, y, z + 1, 1),         ubyte4(v2, v2, v2, 0)},
                {byte4(x + 1, y, z + 1, 1),     ubyte4(v3, v3, v3, 2)},
                {byte4(x + 1, y + 1, z + 1, 1), ubyte4(v4, v4, v4, 3)},

                {byte4(x + 1, y + 1, z + 1, 1), ubyte4(v4, v4, v4, 3)},
                {byte4(x, y + 1, z + 1, 1),     ubyte4(v1, v1, v1, 1)},
                {byte4(x, y, z + 1, 1),         ubyte4(v2, v2, v2, 0)}
        });
    }
    if(faces.left) {
        /*
        v1 = 0;
        v2 = 255/4;
        v3 = 255/2;
        v4 = 255;
         */
        v1 = (u8) (((float) aob.faces[LEFT_FACE].vertices[LEFT_TOP].AO / 3.0f) * fr_l);
        v2 = (u8) (((float) aob.faces[LEFT_FACE].vertices[LEFT_BTM].AO / 3.0f) * fr_l);
        v3 = (u8) (((float) aob.faces[LEFT_FACE].vertices[RIGHT_BTM].AO / 3.0f) * fr_l);
        v4 = (u8) (((float) aob.faces[LEFT_FACE].vertices[RIGHT_TOP].AO / 3.0f) * fr_l);
        vertices.insert(vertices.end(), {
                // Left face
                {byte4(x, y + 1, z + 1, 2), ubyte4(v4, v4, v4, 3)},
                {byte4(x, y + 1, z, 2),     ubyte4(v1, v1, v1, 1)},
                {byte4(x, y, z, 2),         ubyte4(v2, v2, v2, 0)},

                {byte4(x, y, z, 2),         ubyte4(v2, v2, v2, 0)},
                {byte4(x, y, z + 1, 2),     ubyte4(v3, v3, v3, 2)},
                {byte4(x, y + 1, z + 1, 2), ubyte4(v4, v4, v4, 3)}
        });
    }
    if(faces.right) {
        v1 = 0;
        v2 = 255/4;
        v3 = 255/2;
        v4 = 255;
        vertices.insert(vertices.end(), {
                // Right face
                {byte4(x + 1, y + 1, z + 1, 3), ubyte4(v1, v1, v1, 1)},
                {byte4(x + 1, y, z, 3),         ubyte4(v3, v3, v3, 2)},
                {byte4(x + 1, y + 1, z, 3),     ubyte4(v4, v4, v4, 3)},
                {byte4(x + 1, y, z, 3),         ubyte4(v3, v3, v3, 2)},
                {byte4(x + 1, y + 1, z + 1, 3), ubyte4(v1, v1, v1, 1)},
                {byte4(x + 1, y, z + 1, 3),     ubyte4(v2, v2, v2, 0)}
        });
    }
    if(faces.bottom) {
        v1 = 0;
        v2 = 255/4;
        v3 = 255/2;
        v4 = 255;
        vertices.insert(vertices.end(), {
                // Bottom face
                {byte4(x, y, z, 4),         ubyte4(v2, v2, v2, 3)},
                {byte4(x + 1, y, z, 4),     ubyte4(v3, v3, v3, 1)},
                {byte4(x + 1, y, z + 1, 4), ubyte4(v4, v4, v4, 0)},
                {byte4(x + 1, y, z + 1, 4), ubyte4(v4, v4, v4, 0)},
                {byte4(x, y, z + 1, 4),     ubyte4(v1, v1, v1, 2)},
                {byte4(x, y, z, 4),         ubyte4(v2, v2, v2, 3)}
        });
    }
    if(faces.top) {
        v1 = 0;
        v2 = 255/4;
        v3 = 255/2;
        v4 = 255;
        vertices.insert(vertices.end(), {
                // Top face
                {byte4(x, y + 1, z, 5),         ubyte4(v1, v1, v1, 1)},
                {byte4(x + 1, y + 1, z + 1, 5), ubyte4(v3, v3, v3, 2)},
                {byte4(x + 1, y + 1, z, 5),     ubyte4(v4, v4, v4, 3)},
                {byte4(x + 1, y + 1, z + 1, 5), ubyte4(v3, v3, v3, 2)},
                {byte4(x, y + 1, z, 5),         ubyte4(v1, v1, v1, 1)},
                {byte4(x, y + 1, z + 1, 5),     ubyte4(v2, v2, v2, 0)}
        });
    }
}


