//
// Created by bison on 26-12-2017.
//

#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/Mesh3.h>
#include <engine/video/TextureAtlas.h>
#include "BlockTypeDictionary.h"
#include "Block.h"
#include "ChunkPos.h"
#include "Terrain.h"

struct MaterialBlock
{
    float x,y,z;
    MaterialBlock(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct MaterialBatch
{
    int blockType;
    uint32_t start;
    uint32_t count;
    std::vector<MaterialBlock> blocks;
};

class Chunk {
public:
    static const int32_t CHUNK_SIZE = 16;
    static const int32_t CHUNK_HEIGHT = 64;
    Chunk(BlockTypeDictionary &blockTypeDict);

    virtual ~Chunk();

    void rebuild(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain);
    void draw(const Shader& shader);
    glm::vec3 position;
    Block*** blocks;

private:
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<Mesh3> mesh;
    std::map<int32_t, std::unique_ptr<MaterialBatch>> materialBatchMap;

    bool isBlockActiveAt(int32_t x, int32_t y, int32_t z);

    void randomizeHeights();

    void setupFromTerrain(const ChunkPos &position, const std::shared_ptr<Terrain> &terrain);
};


#endif //GAME_TILECHUNK_H
