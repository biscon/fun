//
// Created by bison on 26-12-2017.
//
#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/mesh/Mesh3.h>
#include <engine/video/texture/TextureAtlas.h>
#include "BlockTypeDictionary.h"
#include "Block.h"
#include "ChunkPos.h"
#include "Terrain.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 64
// flatten coords to index
// (z * CHUNK_SIZE * CHUNK_HEIGHT) + (y * CHUNK_SIZE) + x
// (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_HEIGHT) + x
#define POS_TO_INDEX(y,z,x) (y*CHUNK_SIZE)+(z*CHUNK_SIZE*CHUNK_HEIGHT)+x

struct MaterialBlock
{
    float x,y,z;
    MaterialBlock(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct MaterialBatch
{
    int blockType;
    u32 start;
    u32 count;
    std::vector<MaterialBlock> blocks;
};

class Chunk {
public:
    Chunk(BlockTypeDictionary &blockTypeDict);

    virtual ~Chunk();
    void setupFromTerrain(const ChunkPos &position, const std::shared_ptr<Terrain> &terrain);
    void rebuild(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain);
    void draw(const Shader& shader);
    glm::vec3 position;
    // cache friendly order y,z,x
    //Block*** blocks;
    Block *blocks;
    unsigned char lightMap[CHUNK_HEIGHT][CHUNK_SIZE][CHUNK_SIZE];

private:
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<Mesh3> mesh;
    std::map<i32, std::unique_ptr<MaterialBatch>> materialBatchMap;

    inline bool isBlockActiveAt(i32 x, i32 y, i32 z)
    {
        if(x < 0 || x >= CHUNK_SIZE)
            return false;
        if(y < 0 || y >= CHUNK_HEIGHT)
            return false;
        if(z < 0 || z >= CHUNK_SIZE)
            return false;
        return blocks[POS_TO_INDEX(y,z,x)].active;
    }

    // Get the bits XXXX0000
    inline int getSunlight(int x, int y, int z) {
        return (lightMap[y][z][x] >> 4) & 0xF;
    }

    // Set the bits XXXX0000
    inline void setSunlight(int x, int y, int z, int val) {
        lightMap[y][z][x] = (lightMap[y][z][x] & 0xF) | (val << 4);
    }

    // Get the bits 0000XXXX
    inline int getTorchlight(int x, int y, int z) {
        return lightMap[y][z][x] & 0xF;
    }

    // Set the bits 0000XXXX
    inline void setTorchlight(int x, int y, int z, int val) {
        lightMap[y][z][x] = (lightMap[y][z][x] & 0xF0) | val;
    }

    // flatten coords to index
    /*
    inline i32 POS_TO_INDEX(i32 y, i32 z, i32 x)
    {
        return (y*CHUNK_SIZE)+(z*CHUNK_SIZE*CHUNK_HEIGHT)+x;
    }
     */

};


#endif //GAME_TILECHUNK_H
