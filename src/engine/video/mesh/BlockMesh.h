//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MESH3_H
#define GAME_MESH3_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <glm.hpp>
#include <memory>
#include <string>
#include "MeshUpdateType.h"
#include "engine/video/shader/Shader.h"
#include "engine/video/texture/OGLTexture.h"
#include "engine/video/shader/Material.h"
#include "engine/video/texture/TextureAtlas.h"

/*
* Vertex specification (12 bytes per vertex)
* pos: x(i8) y(i8) z(i8) p(i8)     normal: nx(i8) ny(i8) nz(i8) p(i8)    texture: tx(i16) ty(i6)
*
* each attribute takes up 4 bytes
* (which is the min side as I understand EACH attribute must be aligned and not just vertex as a whole
* the padding bytes can be used for other stuff :)
*/

// The alignment of any attribute's data should be no less than 4 bytes. So if you have a vec3 of GLushorts,
// you can't use that 4th component for a new attribute (such as a vec2 of GLbytes).
// If you want to pack something into that instead of having useless padding, you need to make it a vec4 of GLushorts.

// define new glm types
typedef glm::tvec4<GLbyte> byte4;
typedef glm::tvec2<GLshort> short2;
typedef glm::tvec4<GLshort> short4;

struct BlockMeshVertex {
    BlockMeshVertex() {}
    BlockMeshVertex(const byte4 &position, const byte4 &normal, const short2 &texCoord) : position(position),
                                                                                          normal(normal),
                                                                                          texCoord(texCoord) {}

    byte4 position;
    byte4 normal;
    short2 texCoord;
};


class BlockMesh {
public:
    BlockMesh();
    virtual ~BlockMesh();

    void drawRange(const Shader& shader, i32 start, i32 count, Material* material);
    void prepare();
    void upload();
    //void generateCubeAt(float x, float y, float z);
    void generateTexturedCubeAt(i8 x, i8 y, i8 z);
    void clear();

    /*  Mesh Data  */
    std::vector<BlockMeshVertex> vertices;
    std::vector<unsigned int> indices;

    // bytes per vertex
    size_t vertexSize = 8;
    bool hasNormals = true;
    bool hasTexcoords = true;
    MeshUpdateType type = MeshUpdateType::DYNAMIC;

    inline size_t getMeshSizeBytes()
    {
        return vertices.size() * sizeof(BlockMeshVertex);
    }

private:
    void init();
    /*  Render data  */
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    /*  Functions    */
};


#endif //GAME_MESH3_H
