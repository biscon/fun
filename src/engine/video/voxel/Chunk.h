//
// Created by bison on 26-12-2017.
//
#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/mesh/BlockMesh.h>
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
    i8 x,y,z;
    MaterialBlock(i8 x, i8 y, i8 z) : x(x), y(y), z(z) {}
};

struct MaterialBatch
{
    int blockType;
    u32 start;
    u32 count;
    std::vector<MaterialBlock> blocks;
};

class Chunk; // forward

struct ChunkNeighbours {
    Chunk *n;
    Chunk *s;
    Chunk *w;
    Chunk *e;

    ChunkNeighbours() : n(nullptr), s(nullptr), w(nullptr), e(nullptr) {}
    ChunkNeighbours(Chunk *n, Chunk *s, Chunk *w, Chunk *e) : n(n), s(s), w(w), e(e) {}
};

class Chunk {
public:
    Chunk(BlockTypeDictionary &blockTypeDict);

    virtual ~Chunk();
    void setupFromTerrain(const ChunkPos &position, const std::shared_ptr<Terrain> &terrain);
    void rebuild(const ChunkPos& position, ChunkNeighbours& neighbours);
    void draw(const Shader& shader);
    glm::vec3 position;
    // cache friendly order y,z,x
    Block *blocks;
    unsigned char lightMap[CHUNK_HEIGHT][CHUNK_SIZE][CHUNK_SIZE];

    inline size_t getMeshSizeBytes()
    {
        return mesh->getMeshSizeBytes();
    }

private:
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<BlockMesh> mesh;
    std::map<i32, std::unique_ptr<MaterialBatch>> materialBatchMap;

    inline bool isBlockActiveAt(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0 || y >= CHUNK_HEIGHT)
            return false;

        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            return blocks[POS_TO_INDEX(y,z,x)].active;
        }

        // within neighbouring chunk
        // check west
        if(x < 0)
        {
            if(neighbours.w == nullptr)
                return false;
            auto nx = CHUNK_SIZE + x;
            return neighbours.w->blocks[POS_TO_INDEX(y,z,nx)].active;
        }

        // check east
        if(x >= CHUNK_SIZE)
        {
            if(neighbours.e == nullptr)
                return false;

            auto nx = x - CHUNK_SIZE;
            return neighbours.e->blocks[POS_TO_INDEX(y,z,nx)].active;
        }

        // check north
        if(z >= CHUNK_SIZE)
        {
            if(neighbours.n == nullptr)
                return false;

            auto nz = z - CHUNK_SIZE;
            return neighbours.n->blocks[POS_TO_INDEX(y,nz,x)].active;
        }

        // check south
        if(z < 0)
        {
            if(neighbours.s == nullptr)
                return false;
            auto nz = CHUNK_SIZE + z;
            return neighbours.s->blocks[POS_TO_INDEX(y,nz,x)].active;
        }

        return false;
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
