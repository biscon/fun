//
// Created by bison on 26-11-2017.
//

#ifndef GAME_CHUNKMAP_H
#define GAME_CHUNKMAP_H

#define CHUNK_STAGE_SETUP 0
#define CHUNK_STAGE_BUILD 1
#define CHUNK_STAGE_IDLE 2
#define CHUNK_STAGE_REBUILD 3

#include <memory>
#include <vector>
#include "Chunk.h"

class ChunkManager {
public:
    const int CHUNKS_SETUP_PER_FRAME = 2;
    const int CHUNKS_BUILD_PER_FRAME = 2;
    const int VISIBLE_RADIUS = 4;

    ChunkManager(const std::shared_ptr<Terrain> &terrain);

    std::map<ChunkPos, std::unique_ptr<Chunk>> activeChunks;
    std::map<ChunkPos, std::unique_ptr<Chunk>> buildChunks;
    std::vector<std::unique_ptr<Chunk>> recycleList;
    std::vector<Chunk*> rebuildList;

    ChunkPos camChunkPos;
    size_t totalMeshSizeBytes = 0;

    inline Chunk *findActiveChunkAt(const ChunkPos &pos) {
        return findChunkAt(activeChunks, pos);
    }

    inline Chunk *findBuildChunkAt(const ChunkPos &pos) {
        return findChunkAt(buildChunks, pos);
    }

    void worldToChunk(glm::vec3 &worldpos, ChunkPos &chunkpos);
    void chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos);


    inline bool posInVisibleRadius(ChunkPos& pos)
    {
        int dx = pos.x - camChunkPos.x;
        int dy = pos.z - camChunkPos.z;
        return sqrt(dx*dx + dy*dy) <= VISIBLE_RADIUS;
    }

    void runIncrementalChunkBuild();
    void update(glm::vec3& campos, BlockTypeDictionary& blockTypeDict);

    inline std::string getBuildStageAsString() {
        switch(buildStage)
        {
            case CHUNK_STAGE_IDLE:
                return "IDLE";
            case CHUNK_STAGE_SETUP:
                return "SETUP";
            case CHUNK_STAGE_BUILD:
                return "BUILD";
            case CHUNK_STAGE_REBUILD:
                return "REBUILD";
            default:
                return "UNKNOWN";
        }
    }

private:
    float halfChunkSize = (float) CHUNK_SIZE/2;
    float fChunkSize = (float) CHUNK_SIZE;
    std::shared_ptr<Terrain> terrain = nullptr;
    i32 buildStage = CHUNK_STAGE_IDLE;
    std::map<ChunkPos, std::unique_ptr<Chunk>>::iterator setupIterator;

    inline Chunk *findChunkAt(const std::map<ChunkPos, std::unique_ptr<Chunk>>&chunk_map, const ChunkPos &pos) {
        auto it = chunk_map.find(pos);
        if(it != chunk_map.end())
            return (*it).second.get();

        return nullptr;
    }

    void findNeighbours(ChunkNeighbours &neighbours, const ChunkPos& chunk_pos);
    void findNeighbours(const std::map<ChunkPos, std::unique_ptr<Chunk>>&chunk_map, ChunkNeighbours &neighbours, const ChunkPos& chunk_pos);
    void recycleChunks();
    ChunkPos getChunkFromWorld(glm::vec3 &worldpos);
    void createChunks(BlockTypeDictionary& blockTypeDict);

    void determineChunksToRebuild();
};


#endif
