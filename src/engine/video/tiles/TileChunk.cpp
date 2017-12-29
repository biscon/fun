//
// Created by bison on 26-12-2017.
//

#include "TileChunk.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

TileChunk::TileChunk(TileTypeDictionary &tileTypeDict) : tileTypeDict(tileTypeDict) {
    blocks = new Block**[CHUNK_SIZE];
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        blocks[i] = new Block*[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            blocks[i][j] = new Block[CHUNK_SIZE];
        }
    }
}

TileChunk::~TileChunk() {
    // Delete the blocks
    for (int i = 0; i < CHUNK_SIZE; ++i)
    {
        for (int j = 0; j < CHUNK_SIZE; ++j)
        {
            delete [] blocks[i][j];
        }

        delete [] blocks[i];
    }
    delete [] blocks;
}

void TileChunk::rebuild() {
    bool first_build = false;
    if(mesh == nullptr) {
        first_build = true;
        mesh = std::unique_ptr<Mesh3>(new Mesh3());
        mesh->type = MeshUpdateType::DYNAMIC;
        mesh->hasNormals = true;
        mesh->hasTexcoords = true;
        mesh->vertexSize = 8;
        auto material = std::make_shared<Material>();
        material->shininess = 4;
        material->diffuseTexture = tileTypeDict.diffuseAtlas->getTexture();
        material->specularTexture = tileTypeDict.specularAtlas->getTexture();
        mesh->material = material;
    }
    if(!first_build)
        mesh->clear();

    auto m = glm::mat4();
    auto origin = glm::vec4(0,0,0,1);
    m = glm::translate(m, glm::vec3(-0.5*CHUNK_SIZE, 0.5f, -0.5*CHUNK_SIZE));
    for(int z = 0; z < CHUNK_SIZE; z++)
    {
        glm::mat4 old = m;
        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            m = glm::translate(m, glm::vec3(1.0f, 0.0f, 0.0f));
            auto y_m = m;
            //auto top = 12 + (rand() % 4);
            // new material, instantiate new mesh
            for(int y = 0; y < CHUNK_SIZE; y++) {
                Block &block = blocks[x][y][z];
                auto tile = tileTypeDict.getTileTypeAt(block.type);
                auto pos = y_m * origin;
                bool should_mesh = false;

                // Check if each of the cubes side is not active, if then we need to mesh it
                if(!isBlockActiveAt(x,y,z-1)) should_mesh = true; // back
                if(!isBlockActiveAt(x,y,z+1)) should_mesh = true; // front
                if(!isBlockActiveAt(x-1,y,z)) should_mesh = true; // left
                if(!isBlockActiveAt(x+1,y,z)) should_mesh = true; // right
                if(!isBlockActiveAt(x,y+1,z)) should_mesh = true; // bottom
                if(!isBlockActiveAt(x,y-1,z)) should_mesh = true; // top

                if(should_mesh)
                    mesh->generateTexturedCubeAt(pos.x, pos.y, pos.z, tileTypeDict.diffuseAtlas->getUVRect(tile.diffuseMapHandle));

                y_m = glm::translate(y_m, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            //SDL_Log("Generated cube at %.2f,%.2f,%.2f", pos.x, pos.y, pos.z);
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if(first_build)
        mesh->prepare();
    else
        mesh->upload();
}

void TileChunk::draw(const Shader &shader) {
    mesh->draw(shader);
}

bool TileChunk::isBlockActiveAt(int32_t x, int32_t y, int32_t z)
{
    if(x < 0 || x >= CHUNK_SIZE)
        return false;
    if(y < 0 || y >= CHUNK_SIZE)
        return false;
    if(z < 0 || z >= CHUNK_SIZE)
        return false;
    return blocks[x][y][z].active;
}
