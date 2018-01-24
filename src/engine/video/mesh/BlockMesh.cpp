//
// Created by bison on 16-12-2017.
//

#include <engine/util/interpolation.h>
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

static float decelerate(float input)
{
    //return input;
    std::array<float, 4> p;

    p = {{0.015f, 0.10f, 1.0f, 1.5f}};
    return CubicInterpolate(p.data(), input);
}

/*
 * 4 color bytes used as: sunlight, torchlight, ao, texcoord index
 * actual shading must take place in the shader (yeah I know)
 */
void BlockMesh::generateTexturedCubeAt(i8 x, i8 y, i8 z, BlockFaces& faces, BlockLight &blockLight, AmbientOcclusion& aob) {
    /*
    u8 min_level = 1;
    for(i32 i = 0; i < 6; i++)
    {
        if(blockLight.faces[i].v1 < min_level) blockLight.faces[i].v1 = min_level;
        if(blockLight.faces[i].v2 < min_level) blockLight.faces[i].v2 = min_level;
        if(blockLight.faces[i].v3 < min_level) blockLight.faces[i].v3 = min_level;
        if(blockLight.faces[i].v4 < min_level) blockLight.faces[i].v4 = min_level;
    }
    */
    u8 light[6][4];
    for(i32 i = 0; i < 6; i++) {
        light[i][0] = (u8)( decelerate((float)blockLight.sunLight[i].v1 / 15.0f) * 255.0f);
        light[i][1] = (u8)( decelerate((float)blockLight.sunLight[i].v2 / 15.0f) * 255.0f);
        light[i][2] = (u8)( decelerate((float)blockLight.sunLight[i].v3 / 15.0f) * 255.0f);
        light[i][3] = (u8)( decelerate((float)blockLight.sunLight[i].v4 / 15.0f) * 255.0f);
    }

    u8 v1,v2,v3,v4;
    float minLight = 0.3f;
    /*
    if(level == 1)
    {
        SDL_Log("Converted lightlevel %d to rgb intensity %d", level, l);
    }
    */
    if(faces.back) {
        v1 = (u8) (((float) aob.faces[BACK_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[BACK_FACE][0]);
        v2 = (u8) (((float) aob.faces[BACK_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[BACK_FACE][1]);
        v3 = (u8) (((float) aob.faces[BACK_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[BACK_FACE][2]);
        v4 = (u8) (((float) aob.faces[BACK_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[BACK_FACE][3]);
        if(aob.faces[BACK_FACE].vertices[LEFT_BTM].AO + aob.faces[BACK_FACE].vertices[RIGHT_TOP].AO < aob.faces[BACK_FACE].vertices[LEFT_TOP].AO + aob.faces[BACK_FACE].vertices[RIGHT_BTM].AO)
        {
            vertices.insert(vertices.end(), {
                    {byte4(x, y, z, 0),         ubyte4(v3, v3, v3, 0)}, // v2
                    {byte4(x + 1, y + 1, z, 0), ubyte4(v1, v1, v1, 3)}, // v1
                    {byte4(x + 1, y, z, 0),     ubyte4(v2, v2, v2, 2)}, // v2
                    {byte4(x + 1, y + 1, z, 0), ubyte4(v1, v1, v1, 3)}, // v1
                    {byte4(x, y, z, 0),         ubyte4(v3, v3, v3, 0)}, // v3
                    {byte4(x, y + 1, z, 0),     ubyte4(v4, v4, v4, 1)}  // v4
            });
        }
        else
        {
            vertices.insert(vertices.end(), {
                    {byte4(x + 1, y, z, 0),     ubyte4(v2, v2, v2, 2)}, // v2
                    {byte4(x, y, z, 0),         ubyte4(v3, v3, v3, 0)}, // v3
                    {byte4(x, y + 1, z, 0),     ubyte4(v4, v4, v4, 1)}, // v4
                    {byte4(x, y + 1, z, 0),     ubyte4(v4, v4, v4, 1)}, // v4
                    {byte4(x + 1, y + 1, z, 0), ubyte4(v1, v1, v1, 3)}, // v1
                    {byte4(x + 1, y, z, 0),     ubyte4(v2, v2, v2, 2)}  // v2
            });
        }
    }
    if(faces.front)
    {
        v1 = (u8) (((float) aob.faces[FRONT_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[FRONT_FACE][0]);
        v2 = (u8) (((float) aob.faces[FRONT_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[FRONT_FACE][1]);
        v3 = (u8) (((float) aob.faces[FRONT_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[FRONT_FACE][2]);
        v4 = (u8) (((float) aob.faces[FRONT_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[FRONT_FACE][3]);

        if(aob.faces[FRONT_FACE].vertices[LEFT_BTM].AO + aob.faces[FRONT_FACE].vertices[RIGHT_TOP].AO > aob.faces[FRONT_FACE].vertices[LEFT_TOP].AO + aob.faces[FRONT_FACE].vertices[RIGHT_BTM].AO)
        {
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
        else
        {
            vertices.insert(vertices.end(), {
                    // Front face
                    {byte4(x + 1, y, z + 1, 1),     ubyte4(v3, v3, v3, 2)}, // V3
                    {byte4(x, y + 1, z + 1, 1),     ubyte4(v1, v1, v1, 1)}, // V1
                    {byte4(x, y, z + 1, 1),         ubyte4(v2, v2, v2, 0)}, // V2
                    {byte4(x, y + 1, z + 1, 1),     ubyte4(v1, v1, v1, 1)}, // V1
                    {byte4(x + 1, y, z + 1, 1),     ubyte4(v3, v3, v3, 2)}, // V3
                    {byte4(x + 1, y + 1, z + 1, 1), ubyte4(v4, v4, v4, 3)}  // v4
            });
        }
    }
    if(faces.left) {
        v1 = (u8) (((float) aob.faces[LEFT_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[LEFT_FACE][0]);
        v2 = (u8) (((float) aob.faces[LEFT_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[LEFT_FACE][1]);
        v3 = (u8) (((float) aob.faces[LEFT_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[LEFT_FACE][2]);
        v4 = (u8) (((float) aob.faces[LEFT_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[LEFT_FACE][3]);
        if(aob.faces[LEFT_FACE].vertices[LEFT_BTM].AO + aob.faces[LEFT_FACE].vertices[RIGHT_TOP].AO > aob.faces[LEFT_FACE].vertices[LEFT_TOP].AO + aob.faces[LEFT_FACE].vertices[RIGHT_BTM].AO)
        {
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
        else
        {
            vertices.insert(vertices.end(), {
                    // Left face
                    {byte4(x, y + 1, z, 2),     ubyte4(v1, v1, v1, 1)}, // V1
                    {byte4(x, y, z + 1, 2),     ubyte4(v3, v3, v3, 2)}, // V3
                    {byte4(x, y + 1, z + 1, 2), ubyte4(v4, v4, v4, 3)}, // V4
                    {byte4(x, y, z + 1, 2),     ubyte4(v3, v3, v3, 2)}, // V3
                    {byte4(x, y + 1, z, 2),     ubyte4(v1, v1, v1, 1)}, // V1
                    {byte4(x, y, z, 2),         ubyte4(v2, v2, v2, 0)}  // V2
            });
        }
    }
    if(faces.right) {
        v1 = (u8) (((float) aob.faces[RIGHT_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[RIGHT_FACE][0]);
        v2 = (u8) (((float) aob.faces[RIGHT_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[RIGHT_FACE][1]);
        v3 = (u8) (((float) aob.faces[RIGHT_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[RIGHT_FACE][2]);
        v4 = (u8) (((float) aob.faces[RIGHT_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[RIGHT_FACE][3]);
        if(aob.faces[RIGHT_FACE].vertices[LEFT_BTM].AO + aob.faces[RIGHT_FACE].vertices[RIGHT_TOP].AO < aob.faces[RIGHT_FACE].vertices[LEFT_TOP].AO + aob.faces[RIGHT_FACE].vertices[RIGHT_BTM].AO) {
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
        else
        {
            vertices.insert(vertices.end(), {
                    // Right face
                    {byte4(x + 1, y + 1, z, 3),     ubyte4(v4, v4, v4, 3)},
                    {byte4(x + 1, y + 1, z + 1, 3), ubyte4(v1, v1, v1, 1)},
                    {byte4(x + 1, y, z + 1, 3),     ubyte4(v2, v2, v2, 0)},
                    {byte4(x + 1, y, z + 1, 3),     ubyte4(v2, v2, v2, 0)},
                    {byte4(x + 1, y, z, 3),         ubyte4(v3, v3, v3, 2)},
                    {byte4(x + 1, y + 1, z, 3),     ubyte4(v4, v4, v4, 3)}
            });
        }
    }
    if(faces.bottom) {
        v1 = (u8) (((float) aob.faces[BOTTOM_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[BOTTOM_FACE][0]);
        v2 = (u8) (((float) aob.faces[BOTTOM_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[BOTTOM_FACE][1]);
        v3 = (u8) (((float) aob.faces[BOTTOM_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[BOTTOM_FACE][2]);
        v4 = (u8) (((float) aob.faces[BOTTOM_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[BOTTOM_FACE][3]);

        if(aob.faces[BOTTOM_FACE].vertices[LEFT_BTM].AO + aob.faces[BOTTOM_FACE].vertices[RIGHT_TOP].AO > aob.faces[BOTTOM_FACE].vertices[LEFT_TOP].AO + aob.faces[BOTTOM_FACE].vertices[RIGHT_BTM].AO) {
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
        else
        {
            vertices.insert(vertices.end(), {
                    // Bottom face
                    {byte4(x, y, z + 1, 4),     ubyte4(v1, v1, v1, 2)}, // V1
                    {byte4(x + 1, y, z, 4),     ubyte4(v3, v3, v3, 1)}, // V3
                    {byte4(x + 1, y, z + 1, 4), ubyte4(v4, v4, v4, 0)}, // V4
                    {byte4(x + 1, y, z, 4),     ubyte4(v3, v3, v3, 1)}, // V3
                    {byte4(x, y, z + 1, 4),     ubyte4(v1, v1, v1, 2)}, // V1
                    {byte4(x, y, z, 4),         ubyte4(v2, v2, v2, 3)}  // V2
            });
        }
    }
    if(faces.top) {
        v1 = (u8) (((float) aob.faces[TOP_FACE].vertices[LEFT_TOP].AO / 3.0f) * light[TOP_FACE][0]);
        v2 = (u8) (((float) aob.faces[TOP_FACE].vertices[LEFT_BTM].AO / 3.0f) * light[TOP_FACE][1]);
        v3 = (u8) (((float) aob.faces[TOP_FACE].vertices[RIGHT_BTM].AO / 3.0f) * light[TOP_FACE][2]);
        v4 = (u8) (((float) aob.faces[TOP_FACE].vertices[RIGHT_TOP].AO / 3.0f) * light[TOP_FACE][3]);

        if(aob.faces[TOP_FACE].vertices[LEFT_BTM].AO + aob.faces[TOP_FACE].vertices[RIGHT_TOP].AO < aob.faces[TOP_FACE].vertices[LEFT_TOP].AO + aob.faces[TOP_FACE].vertices[RIGHT_BTM].AO) {
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
        else
        {
            vertices.insert(vertices.end(), {
                    {byte4(x, y + 1, z + 1, 5),     ubyte4(v2, v2, v2, 0)}, // v2
                    {byte4(x + 1, y + 1, z + 1, 5), ubyte4(v3, v3, v3, 2)}, // v3
                    {byte4(x + 1, y + 1, z, 5),     ubyte4(v4, v4, v4, 3)}, // v4
                    {byte4(x + 1, y + 1, z, 5),     ubyte4(v4, v4, v4, 3)}, // v4
                    {byte4(x, y + 1, z, 5),         ubyte4(v1, v1, v1, 1)}, // v1
                    {byte4(x, y + 1, z + 1, 5),     ubyte4(v2, v2, v2, 0)}  // v2

            });
        }
    }
}


