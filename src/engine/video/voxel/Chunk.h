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
#include "IChunkManager.h"

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

    Chunk *nw;
    Chunk *ne;
    Chunk *sw;
    Chunk *se;

    ChunkNeighbours() : n(nullptr), s(nullptr), w(nullptr), e(nullptr), nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}
    ChunkNeighbours(Chunk *n, Chunk *s, Chunk *w, Chunk *e, Chunk *nw, Chunk *ne, Chunk *sw, Chunk *se) : n(n), s(s), w(w), e(e), nw(nw), ne(ne), sw(sw), se(se) {}
};


class Chunk {
public:
    Chunk(BlockTypeDictionary &blockTypeDict, IChunkManager *chunkManager);
    virtual ~Chunk();
    void setupDebugChunk();
    void setupFromTerrain(const ChunkPos &position, const std::shared_ptr<Terrain> &terrain);
    void rebuild(const ChunkPos& position);
    void draw(const Shader& shader);
    glm::vec3 position;
    // cache friendly order y,z,x
    Block *blocks;
    u8 *lightMap;

    inline size_t getMeshSizeBytes()
    {
        return mesh->getMeshSizeBytes();
    }

    ChunkPos chunkPosition;

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
        return lightMap[POS_TO_INDEX(y,z,x)];
    }

    // Set the bits 0000XXXX
    inline void setTorchlight(int x, int y, int z, int val) {
        lightMap[POS_TO_INDEX(y,z,x)] = (u8) val;
    }

private:
    IChunkManager *chunkManager;
    i32 lightMapSize = 0;
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<BlockMesh> mesh;
    std::map<i32, std::unique_ptr<MaterialBatch>> materialBatchMap;

    // function cannot handle lookups in diagonal chunk neighbours
    inline bool shouldBlockMeshAt(i32 x, i32 y, i32 z)
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

        ChunkBlockPos cbpos;
        chunkManager->relativePosToChunkBlockPos(this, x, y, z, cbpos);
        if(cbpos.chunk == nullptr)
            return true;
        return cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_SHOULD_MESH);
    }

    inline bool isBlockActiveAt(i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0)
            return true;
        if(y >= CHUNK_HEIGHT)
            return false;

        // within this chunk
        if(x >= 0 && x < CHUNK_SIZE
           && y >= 0 && y < CHUNK_HEIGHT
           && z >= 0 && z < CHUNK_SIZE) {
            return blocks[POS_TO_INDEX(y, z, x)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        ChunkBlockPos cbpos;
        chunkManager->relativePosToChunkBlockPos(this, x, y, z, cbpos);
        if(cbpos.chunk == nullptr)
            return true;
        return cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    inline u8 getTorchLightAt(i32 x, i32 y, i32 z) {
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

        ChunkBlockPos cbpos;
        chunkManager->relativePosToChunkBlockPos(this, x, y, z, cbpos);
        if(cbpos.chunk == nullptr)
            return 0;
        return cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z);
    }

    inline u8 getTorchLightAndIncreaseCount(i32 x, i32 y, i32 z, i32 &count) {
        if(!isBlockActiveAt(x, y, z))
        {
            count++;
            return getTorchLightAt(x, y, z);
        }
        return 0;
    }

    void clearLightMap();
    void calculateAO(AOBlock &aob, MaterialBlock &mb);

    void calculateBlockLight(BlockLight &block_light, MaterialBlock &mb);
};


#endif //GAME_TILECHUNK_H
