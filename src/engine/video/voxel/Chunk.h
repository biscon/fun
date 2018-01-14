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

    inline Chunk *getChunkByOffset(i32 offset_x, i32 offset_z, i32 x, i32 y, i32 z, i32& nx, i32& nz)
    {
        // if north
        if(offset_x == 0 && offset_z == -1)
        {
            if(n == nullptr)
                return nullptr;
            nx = x;
            nz = CHUNK_SIZE + z;
            return n;
        }

        // if south
        if(offset_x == 0 && offset_z == 1)
        {
            if(s == nullptr)
                return nullptr;
            nx = x;
            nz = z - CHUNK_SIZE;
            return s;
        }

        // if west
        if(offset_x == -1 && offset_z == 0)
        {
            if(w == nullptr)
                return nullptr;
            nx = CHUNK_SIZE + x;
            nz = z;
            return w;
        }

        // if east
        if(offset_x == 1 && offset_z == 0)
        {
            if(e == nullptr)
                return nullptr;

            nx = x - CHUNK_SIZE;
            nz = z;
            return e;
        }

        // diagonals -------------------------------------------------------------------------

        // if north west
        if(offset_x == -1 && offset_z == -1)
        {
            if(nw == nullptr)
                return nullptr;
            nx = CHUNK_SIZE + x;
            nz = CHUNK_SIZE + z;
            return nw;
        }

        // if north east
        if(offset_x == 1 && offset_z == -1) {
            if (ne == nullptr)
                return nullptr;
            nz = CHUNK_SIZE + z;
            nx = x - CHUNK_SIZE;
            return ne;
        }

        // if south west
        if(offset_x == -1 && offset_z == 1)
        {
            if(sw == nullptr)
                return nullptr;
            nx = CHUNK_SIZE + x;
            nz = z - CHUNK_SIZE;
            return sw;
        }

        // if south east
        if(offset_x == 1 && offset_z == 1)
        {
            if(se == nullptr)
                return nullptr;
            auto nx = x - CHUNK_SIZE;
            auto nz = z - CHUNK_SIZE;
            return se;
        }
    }
};

struct LightNode {
    LightNode(Chunk* chunk, i32 x, i32 y, i32 z) : chunk(chunk), x(x), y(y), z(z) {}
    Chunk* chunk;
    i32 x,y,z;
};

class Chunk {
public:
    Chunk(BlockTypeDictionary &blockTypeDict, IChunkManager *chunkManager);
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

private:
    IChunkManager *chunkManager;
    i32 lightMapSize = 0;
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<BlockMesh> mesh;
    std::map<i32, std::unique_ptr<MaterialBatch>> materialBatchMap;

    // function cannot handle lookups in diagonal chunk neighbours
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

        // within this chunk
        if(x >= 0 && x < CHUNK_SIZE
           && y >= 0 && y < CHUNK_HEIGHT
           && z >= 0 && z < CHUNK_SIZE) {
            return blocks[POS_TO_INDEX(y, z, x)].isFlagSet(BLOCK_FLAG_ACTIVE);
        }

        // find chunk neighbour offset
        i32 offset_x = 0;
        i32 offset_z = 0;
        // north
        if(z < 0) {
            offset_z = -1;
        }
        // south
        if(z >= CHUNK_SIZE)
        {
            offset_z = 1;
        }
        // west
        if(x < 0) {
            offset_x = -1;
        }
        // east
        if(x >= CHUNK_SIZE)
        {
            offset_x = 1;
        }

        i32 nx = 0;
        i32 nz = 0;
        Chunk* chunk = neighbours.getChunkByOffset(offset_x, offset_z, x, y, z, nx, nz);
        if(chunk == nullptr)
            return false;
        return chunk->blocks[POS_TO_INDEX(y,nz,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    inline Block* getBlockAt(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z)
    {
        // early out since our chunk grid is 2d
        if(y < 0)
            return nullptr;
        if(y >= CHUNK_HEIGHT)
            return nullptr;

        // within this chunk
        if(x >= 0 && x < CHUNK_SIZE
           && y >= 0 && y < CHUNK_HEIGHT
           && z >= 0 && z < CHUNK_SIZE) {
            return &blocks[POS_TO_INDEX(y, z, x)];
        }

        // find chunk neighbour offset
        i32 offset_x = 0;
        i32 offset_z = 0;
        // north
        if(z < 0) {
            offset_z = -1;
        }
        // south
        if(z >= CHUNK_SIZE)
        {
            offset_z = 1;
        }
        // west
        if(x < 0) {
            offset_x = -1;
        }
        // east
        if(x >= CHUNK_SIZE)
        {
            offset_x = 1;
        }

        i32 nx = 0;
        i32 nz = 0;
        Chunk* chunk = neighbours.getChunkByOffset(offset_x, offset_z, x, y, z, nx, nz);
        if(chunk == nullptr)
            return nullptr;
        return &chunk->blocks[POS_TO_INDEX(y,nz,nx)];
    }

    inline u8 getTorchLightAt(ChunkNeighbours &neighbours, i32 x, i32 y, i32 z) {
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


        // find chunk neighbour offset
        i32 offset_x = 0;
        i32 offset_z = 0;
        // north
        if(z < 0) {
            offset_z = -1;
        }
        // south
        if(z >= CHUNK_SIZE)
        {
            offset_z = 1;
        }
        // west
        if(x < 0) {
            offset_x = -1;
        }
        // east
        if(x >= CHUNK_SIZE)
        {
            offset_x = 1;
        }

        i32 nx = 0;
        i32 nz = 0;
        Chunk* chunk = neighbours.getChunkByOffset(offset_x, offset_z, x, y, z, nx, nz);
        if(chunk == nullptr)
            return 0;
        return chunk->getTorchlight(nz, y, nz);
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

};


#endif //GAME_TILECHUNK_H
