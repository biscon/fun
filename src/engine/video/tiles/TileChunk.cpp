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

TileChunk::TileChunk(TileTypeDictionary &tileTypeDict) : tileTypeDict(tileTypeDict) {}

void TileChunk::rebuild() {
    std::array<uint32_t, 16*16> tiles = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,
            0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,3,0,0,1,1,1,1,1,1,0,0,3,0,0,
            0,3,3,3,0,1,1,1,1,1,1,0,3,3,3,0,
            0,0,3,0,0,1,1,1,1,1,1,0,0,3,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
            0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,
            0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,
            0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    mesh = std::unique_ptr<Mesh3>(new Mesh3());
    mesh->type = MeshUpdateType::DYNAMIC;
    mesh->hasNormals = true;
    mesh->hasTexcoords = true;
    mesh->vertexSize = 8;

    auto material = std::make_shared<Material>();
    material->shininess = 4;
    material->diffuseTexture = tileTypeDict.diffuseAtlas->getTexture();
    material->specularTexture = tileTypeDict.specularAtlas->getTexture();
    auto m = glm::mat4();
    auto origin = glm::vec4(0,0,0,1);
    m = glm::translate(m, glm::vec3(-0.5*CHUNK_SIZE, -0.5f, -0.5*CHUNK_SIZE));
    for(int y = 0; y < CHUNK_SIZE; y++)
    {
        glm::mat4 old = m;
        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            m = glm::translate(m, glm::vec3(1.0f, 0.0f, 0.0f));
            auto pos = m * origin;
            //pos.y = ((float) rand()/ (float) RAND_MAX) / 4.0f;
            auto type = tiles[y*16+x];
            auto tile = tileTypeDict.getTileTypeAt(type);

            // new material, instantiate new mesh
            mesh->generateTexturedCubeAt(pos.x, pos.y, pos.z, tileTypeDict.diffuseAtlas->getUVRect(tile.diffuseMapHandle));
            //SDL_Log("Generated cube at %.2f,%.2f,%.2f", pos.x, pos.y, pos.z);
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    mesh->material = material;
    mesh->prepare();
}

void TileChunk::draw(const Shader &shader) {
    mesh->draw(shader);
}

