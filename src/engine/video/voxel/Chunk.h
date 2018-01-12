//
// Created by bison on 26-12-2017.
//
#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/mesh/BlockMesh.h>
#include <engine/video/texture/TextureAtlas.h>
#include <queue>
#include "BlockTypeDictionary.h"
#include "Block.h"
#include "ChunkPos.h"
#include "Terrain.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
// flatten coords to index
// (z * CHUNK_SIZE * CHUNK_HEIGHT) + (y * CHUNK_SIZE) + x
// (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_HEIGHT) + x
#define POS_TO_INDEX(y,z,x) (y*CHUNK_SIZE)+(z*CHUNK_SIZE*CHUNK_HEIGHT)+x

struct MaterialBlock
{
    i8 x,y,z,l;
    BlockFaces faces;
    MaterialBlock(i8 x, i8 y, i8 z, i8 l) : x(x), y(y), z(z), l(l) {}
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

struct LightNode {
    LightNode(u16 indx, Chunk* ch) : index(indx), chunk(ch) {}
    u16 index;
    Chunk* chunk;
};

class Chunk {
public:
    Chunk(BlockTypeDictionary &blockTypeDict);
    virtual ~Chunk();
    void setupDebugChunk();
    void setupFromTerrain(const ChunkPos &position, const std::shared_ptr<Terrain> &terrain);
    void rebuild(const ChunkPos& position, ChunkNeighbours& neighbours);
    void draw(const Shader& shader);
    glm::vec3 position;
    // cache friendly order y,z,x
    Block *blocks;
    u8 *lightMap;
    std::queue <LightNode> lightQueue;

    inline size_t getMeshSizeBytes()
    {
        return mesh->getMeshSizeBytes();
    }

    ChunkPos chunkPosition;
    void placeTorchLight(int x, int y, int z, u8 level);

private:
    i32 lightMapSize = 0;
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<BlockMesh> mesh;
    std::map<i32, std::unique_ptr<MaterialBatch>> materialBatchMap;

    inline bool shouldBlockMeshAt(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0)
            return false;
        if(y >= CHUNK_HEIGHT)
            return false;

        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            return blocks[POS_TO_INDEX(y,z,x)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
        }


        // within neighbouring chunk
        // check west
        if(x < 0)
        {
            if(neighbours.w == nullptr)
                return true;
            auto nx = CHUNK_SIZE + x;
            return neighbours.w->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
        }

        // check east
        if(x >= CHUNK_SIZE)
        {
            if(neighbours.e == nullptr)
                return true;

            auto nx = x - CHUNK_SIZE;
            return neighbours.e->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
        }

        // check south
        if(z >= CHUNK_SIZE)
        {
            if(neighbours.s == nullptr)
                return true;

            auto nz = z - CHUNK_SIZE;
            return neighbours.s->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
        }

        // check north
        if(z < 0)
        {
            if(neighbours.n == nullptr)
                return true;
            auto nz = CHUNK_SIZE + z;
            return neighbours.n->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
        }

        return true;
    }

    inline bool isBlockActiveAt(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0)
            return true;
        if(y >= CHUNK_HEIGHT)
            return false;

        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            return blocks[POS_TO_INDEX(y,z,x)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        // within neighbouring chunk
        // check west
        if(x < 0)
        {
            if(neighbours.w == nullptr)
                return true;
            auto nx = CHUNK_SIZE + x;
            return neighbours.w->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        // check east
        if(x >= CHUNK_SIZE)
        {
            if(neighbours.e == nullptr)
                return true;

            auto nx = x - CHUNK_SIZE;
            return neighbours.e->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        // check south
        if(z >= CHUNK_SIZE)
        {
            if(neighbours.s == nullptr)
                return true;

            auto nz = z - CHUNK_SIZE;
            return neighbours.s->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        // check north
        if(z < 0)
        {
            if(neighbours.n == nullptr)
                return true;
            auto nz = CHUNK_SIZE + z;
            return neighbours.n->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        return false;
    }

    inline u8 getLightTorchLightAt(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z) {
        // early out since our chunk grid is 2d
        if(y < 0)
            return 0;
        if(y >= CHUNK_HEIGHT)
            return 0;

        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            return getTorchlight(x,y,z);
        }

        // within neighbouring chunk
        // check west
        if(x < 0)
        {
            if(neighbours.w == nullptr)
                return 0;
            auto nx = CHUNK_SIZE + x;
            return static_cast<u8>(neighbours.w->getTorchlight(nx, y, z));
        }

        // check east
        if(x >= CHUNK_SIZE)
        {
            if(neighbours.e == nullptr)
                return 0;

            auto nx = x - CHUNK_SIZE;
            return static_cast<u8>(neighbours.e->getTorchlight(nx, y, z));
        }

        // check south
        if(z >= CHUNK_SIZE)
        {
            if(neighbours.s == nullptr)
                return 0;

            auto nz = z - CHUNK_SIZE;
            return static_cast<u8>(neighbours.s->getTorchlight(x, y, nz));
        }

        // check north
        if(z < 0)
        {
            if(neighbours.n == nullptr)
                return 0;
            auto nz = CHUNK_SIZE + z;
            return static_cast<u8>(neighbours.n->getTorchlight(x, y, nz));
        }

        return 0;
    }

    // Get the bits XXXX0000
    inline int getSunlight(int x, int y, int z) {
        return (lightMap[POS_TO_INDEX(y,z,x)] >> 4) & 0xF;
    }

    // Set the bits XXXX0000
    inline void setSunlight(int x, int y, int z, int val) {
        lightMap[POS_TO_INDEX(y,z,x)] = (lightMap[POS_TO_INDEX(y,z,x)] & 0xF) | (val << 4);
    }

    // Get the bits 0000XXXX
    inline int getTorchlight(int x, int y, int z) {
        return lightMap[POS_TO_INDEX(y,z,x)] & 0xF;
    }

    // Set the bits 0000XXXX
    inline void setTorchlight(int x, int y, int z, int val) {
        lightMap[POS_TO_INDEX(y,z,x)] = (lightMap[POS_TO_INDEX(y,z,x)] & 0xF0) | val;
    }


    inline void clearLightQueue()
    {
        std::queue<LightNode> empty;
        std::swap(lightQueue, empty);
    }
    // flatten coords to index
    /*
    inline i32 POS_TO_INDEX(i32 y, i32 z, i32 x)
    {
        return (y*CHUNK_SIZE)+(z*CHUNK_SIZE*CHUNK_HEIGHT)+x;
    }
     */

    void propagateTorchLight(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel);
    void clearLightMap();

    void calculateAO(ChunkNeighbours &neighbours, AOBlock &aob, MaterialBlock &mb);

    bool isBlockActiveAtClipped(ChunkNeighbours &neighbours, int32_t x, int32_t y, int32_t z);
};


#endif //GAME_TILECHUNK_H
