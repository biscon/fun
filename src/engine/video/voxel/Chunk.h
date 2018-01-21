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
    i8 x,y,z;
    BlockFaces faces;
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

    /*
    // Get the bits 0000XXXX
    inline int getTorchlight(int x, int y, int z) {
        return lightMap[POS_TO_INDEX(y,z,x)];
    }

    // Set the bits 0000XXXX
    inline void setTorchlight(int x, int y, int z, int val) {
        lightMap[POS_TO_INDEX(y,z,x)] = (u8) val;
    }
    */

    // Get the bits 0000XXXX
    inline int getTorchlight(int x, int y, int z) {
        return lightMap[POS_TO_INDEX(y,z,x)] & 0xF;
    }

    // Set the bits 0000XXXX
    inline void setTorchlight(int x, int y, int z, int val) {
        lightMap[POS_TO_INDEX(y,z,x)] = (lightMap[POS_TO_INDEX(y,z,x)] & 0xF0) | val;
    }

    void clearLightMap();

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

    inline u8 getSunLightAt(i32 x, i32 y, i32 z) {
        // early out since our chunk grid is 2d
        if(y < 0)
            return 0;
        if(y >= CHUNK_HEIGHT)
            return 15;

        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            return getSunlight(x,y,z);
        }

        return 0;

        ChunkBlockPos cbpos;
        chunkManager->relativePosToChunkBlockPos(this, x, y, z, cbpos);
        if(cbpos.chunk == nullptr) {
            SDL_Log("Could not find chunk at %d,%d,%d", x, y, z);
            return 0;
        }
        return cbpos.chunk->getSunlight(cbpos.x, cbpos.y, cbpos.z);
    }

    inline bool isBlockTransparentAt(i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0)
            return false;
        if(y >= CHUNK_HEIGHT)
            return true;

        // within this chunk
        if(x >= 0 && x < CHUNK_SIZE
           && y >= 0 && y < CHUNK_HEIGHT
           && z >= 0 && z < CHUNK_SIZE) {
            return blocks[POS_TO_INDEX(y, z, x)].isFlagSet(BLOCK_FLAG_TRANSPARENT);
        }

        ChunkBlockPos cbpos;
        chunkManager->relativePosToChunkBlockPos(this, x, y, z, cbpos);
        if(cbpos.chunk == nullptr)
            return false;
        return cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_TRANSPARENT);
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



    inline u8 getTorchLightAndIncreaseCount(i32 x, i32 y, i32 z, i32 &count) {
        if(!isBlockActiveAt(x, y, z))
        {
            count++;
            return getSunLightAt(x, y, z);
        }
        return 0;
    }

    void calculateAO(AmbientOcclusion &aob, MaterialBlock &mb);

    void calculateBlockLight(BlockLight &block_light, MaterialBlock &mb);

    void calculateBlockyLight(BlockLight &block_light, MaterialBlock &mb);
};


#endif //GAME_TILECHUNK_H
