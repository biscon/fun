//
// Created by bison on 26-12-2017.
//

#include "Chunk.h"

#define GLEW_STATIC

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// cache friendly order y,z,x
Chunk::Chunk(BlockTypeDictionary &blockTypeDict) : blockTypeDict(blockTypeDict) {

    blocks = (Block*) malloc(CHUNK_HEIGHT*CHUNK_SIZE*CHUNK_SIZE*sizeof(Block));
    //memset(blocks, 0, sizeof(blocks));
    // clear lightmap
    memset(lightMap, 0, sizeof(lightMap));
}

Chunk::~Chunk() {
    // Delete the blocks
    free(blocks);
}

// cache friendly order y,z,x
void Chunk::setupFromTerrain(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain)
{
    int half_terrain_size = terrain->getHeightMap()->getWidth()/2;
    int half_chunk_size = CHUNK_SIZE/2;
    float fchunk_height = (float) CHUNK_HEIGHT;

    // to map coords
    auto chk_map_x = (position.x * CHUNK_SIZE) + half_terrain_size;
    auto chk_map_z = (position.z * CHUNK_SIZE) + half_terrain_size;
    //SDL_Log("Chunkpos %d,%d mapped to chunk map pos %d,%d", position.x, position.z, chk_map_x, chk_map_z);

    for(int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            //SDL_Log("Sampled height %d at map pos %d,%d", height, map_x, map_z);
            for(int x = 0; x < CHUNK_SIZE; x++)
            {
                auto map_x = chk_map_x + x - half_chunk_size;
                auto map_z = chk_map_z + z - half_chunk_size;
                auto height = (int) ceil(terrain->getHeightMap()->getSampleWrap(map_x, map_z) * fchunk_height);

                //SDL_Log("(%d,%d,%d) Index = %d", y, z, x, POS_TO_INDEX(y,z,x));
                blocks[POS_TO_INDEX(y,z,x)].active = false;
                blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GRASS;
                if(y <= height)
                {
                    blocks[POS_TO_INDEX(y,z,x)].active = true;
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GRASS;
                    if(y >= 0 && y <= 8)
                        blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_STONE;

                }
                if(y > height && y <= 4)
                {
                    blocks[POS_TO_INDEX(y,z,x)].active = true;
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_WATER;
                }
            }
        }
    }
}

// cache friendly order y,z,x
void Chunk::rebuild(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain) {
    // clear lightmap
    memset(lightMap, 0, sizeof(lightMap));

    bool first_build = false;
    if(mesh == nullptr) {
        first_build = true;
        mesh = std::unique_ptr<BlockMesh>(new BlockMesh());
    }
    if(!first_build)
        mesh->clear();

    //setupFromTerrain(position, terrain);
    //randomizeHeights();
    materialBatchMap.clear();

    auto m = glm::mat4();
    auto origin = glm::vec4(0,0,0,1);
    m = glm::translate(m, glm::vec3(-0.5*CHUNK_SIZE, 0.0f, -0.5*CHUNK_SIZE));
    for(int y = 0; y < CHUNK_HEIGHT; y++)
    {
        glm::mat4 old = m;
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            m = glm::translate(m, glm::vec3(0.0f, 0.0f, 1.0f));
            auto z_m = m;
            for(int x = 0; x < CHUNK_SIZE; x++) {
                Block &block = blocks[POS_TO_INDEX(y,z,x)];
                if(block.active) {
                    auto pos = z_m * origin;
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
                            materialBatchMap[block.type]->blocks.emplace_back(x,y,z);
                            //SDL_Log("Generated cube at %d,%d,%d", x, y, z);
                        }
                        else {
                            batch->second->blocks.emplace_back(x, y, z);
                            //SDL_Log("Generated cube at %d,%d,%d", x, y, z);
                        }
                    }
                }
                z_m = glm::translate(z_m, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    for(auto const& kv : materialBatchMap)
    {
        kv.second->start = static_cast<u32>(mesh->vertices.size());
        for(auto const& mb : kv.second->blocks)
        {
            //SDL_Log("Meshing cube at %d,%d,%d", mb.x, mb.y, mb.z);
            mesh->generateTexturedCubeAt(mb.x, mb.y, mb.z);
        }
        kv.second->count = static_cast<u32>(mesh->vertices.size() - kv.second->start);
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

