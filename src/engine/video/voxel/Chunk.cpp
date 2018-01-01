//
// Created by bison on 26-12-2017.
//

#include "Chunk.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

Chunk::Chunk(BlockTypeDictionary &blockTypeDict) : blockTypeDict(blockTypeDict) {
    blocks = new Block**[CHUNK_SIZE];
    for(int x = 0; x < CHUNK_SIZE; x++)
    {
        blocks[x] = new Block*[CHUNK_HEIGHT];
        for(int y = 0; y < CHUNK_HEIGHT; y++)
        {
            blocks[x][y] = new Block[CHUNK_SIZE];
        }
    }
}

Chunk::~Chunk() {
    // Delete the blocks
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            delete [] blocks[x][y];
        }

        delete [] blocks[x];
    }
    delete [] blocks;
}

void Chunk::randomizeHeights()
{
    for(int z = 0; z < CHUNK_SIZE; z++)
    {
        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            blocks[x][0][z].active = true;
            blocks[x][0][z].type = Block::STONE;
            blocks[x][5][z].active = true;
            blocks[x][5][z].type = Block::GRASS;
            blocks[x][10][z].active = true;
            blocks[x][10][z].type = Block::WATER;

            blocks[x][15][z].active = true;
            blocks[x][15][z].type = 3;
            blocks[x][20][z].active = true;
            blocks[x][20][z].type = 4;
            blocks[x][25][z].active = true;
            blocks[x][25][z].type = 5;
        }
    }
}

void Chunk::setupFromTerrain(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain)
{
    int half_terrain_size = terrain->getHeightMap()->getWidth()/2;
    int half_chunk_size = CHUNK_SIZE/2;
    float fchunk_height = (float) CHUNK_HEIGHT;

    // to map coords
    auto chk_map_x = (position.x * CHUNK_SIZE) + half_terrain_size;
    auto chk_map_z = (position.z * CHUNK_SIZE) + half_terrain_size;
    //SDL_Log("Chunkpos %d,%d mapped to chunk map pos %d,%d", position.x, position.z, chk_map_x, chk_map_z);

    for(int z = 0; z < CHUNK_SIZE; z++)
    {
        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            auto map_x = chk_map_x + x - half_chunk_size;
            auto map_z = chk_map_z + z - half_chunk_size;
            auto height = (int) ceil(terrain->getHeightMap()->getSampleWrap(map_x, map_z) * fchunk_height);
            //SDL_Log("Sampled height %d at map pos %d,%d", height, map_x, map_z);
            for(int y = 0; y < CHUNK_HEIGHT; y++)
            {
                blocks[x][y][z].active = false;
                blocks[x][y][z].type = Block::GRASS;
                if(y <= height)
                {
                    blocks[x][y][z].active = true;
                    blocks[x][y][z].type = Block::GRASS;
                    if(y >= 0 && y <= 8)
                        blocks[x][y][z].type = Block::STONE;

                }
                if(y > height && y <= 4)
                {
                    blocks[x][y][z].active = true;
                    blocks[x][y][z].type = Block::WATER;
                }
            }
        }
    }
}

void Chunk::rebuild(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain) {
    bool first_build = false;
    if(mesh == nullptr) {
        first_build = true;
        mesh = std::unique_ptr<Mesh3>(new Mesh3());
        mesh->type = MeshUpdateType::DYNAMIC;
        mesh->hasNormals = true;
        mesh->hasTexcoords = true;
        mesh->vertexSize = 8;
    }
    if(!first_build)
        mesh->clear();

    setupFromTerrain(position, terrain);
    //randomizeHeights();
    materialBatchMap.clear();

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
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                Block &block = blocks[x][y][z];
                if(block.active) {
                    //auto blockType = blockTypeDict.getBlockTypeAt(block.type);
                    auto pos = y_m * origin;
                    bool should_mesh = false;

                    // Check if each of the cubes side is not active, if then we need to mesh it
                    if (!isBlockActiveAt(x, y, z - 1)) should_mesh = true; // back
                    if (!isBlockActiveAt(x, y, z + 1)) should_mesh = true; // front
                    if (!isBlockActiveAt(x - 1, y, z)) should_mesh = true; // left
                    if (!isBlockActiveAt(x + 1, y, z)) should_mesh = true; // right
                    if (!isBlockActiveAt(x, y + 1, z)) should_mesh = true; // bottom
                    if (!isBlockActiveAt(x, y - 1, z)) should_mesh = true; // top

                    if (should_mesh) {
                        auto batch = materialBatchMap.find(block.type);
                        if(batch == materialBatchMap.end()) // batch does not exist, create
                        {
                            materialBatchMap[block.type] = std::unique_ptr<MaterialBatch>(new MaterialBatch());
                            materialBatchMap[block.type]->blockType = block.type;
                            materialBatchMap[block.type]->blocks.emplace_back(pos.x, pos.y, pos.z);
                        }
                        else
                            batch->second->blocks.emplace_back(pos.x, pos.y, pos.z);
                    }
                }
                y_m = glm::translate(y_m, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            //SDL_Log("Generated cube at %.2f,%.2f,%.2f", pos.x, pos.y, pos.z);
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    UVRect uvRect;
    uvRect.left = 0.0f;
    uvRect.right = 1.0f;
    uvRect.top = 1.0f;
    uvRect.bottom = 0.0f;
    for(auto const& kv : materialBatchMap)
    {
        kv.second->start = mesh->vertices.size();
        for(auto const& mb : kv.second->blocks)
        {
            mesh->generateTexturedCubeAt(mb.x, mb.y, mb.z, uvRect);
        }
        kv.second->count = mesh->vertices.size() - kv.second->start;
    }
    if(first_build)
        mesh->prepare();
    else
        mesh->upload();
}

void Chunk::draw(const Shader &shader) {
    //SDL_Log("Rendering %d batches", materialBatchMap.size());
    //mesh->drawRange(shader, 0, mesh->vertices.size(), &blockTypeDict.getBlockTypeAt(0).material);

    for(auto const& kv : materialBatchMap)
    {
        //sum += kv.second->count;
        //SDL_Log("Rendering batch: blocks = %d, start = %d, count = %d, blockType = %d", kv.second->blocks.size(), kv.second->start, kv.second->count, kv.first);
        mesh->drawRange(shader, kv.second->start, kv.second->count, &blockTypeDict.getBlockTypeAt(kv.first).material);
    }

    //SDL_Log("Sum of batch counts = %d, size of mesh = %d", sum, mesh->vertices.size());
}

bool Chunk::isBlockActiveAt(int32_t x, int32_t y, int32_t z)
{
    if(x < 0 || x >= CHUNK_SIZE)
        return false;
    if(y < 0 || y >= CHUNK_HEIGHT)
        return false;
    if(z < 0 || z >= CHUNK_SIZE)
        return false;
    return blocks[x][y][z].active;
}
