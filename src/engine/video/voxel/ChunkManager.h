//
// Created by bison on 26-11-2017.
//

#ifndef GAME_CHUNKMAP_H
#define GAME_CHUNKMAP_H

#define CHUNK_STAGE_SETUP 0
#define CHUNK_STAGE_BUILD 1

#include <memory>
#include <vector>
#include "Chunk.h"

class ChunkManager {
public:
    const int CHUNKS_SETUP_PER_FRAME = 2;
    const int CHUNKS_BUILD_PER_FRAME = 2;
    const int VISIBLE_RADIUS = 24;

    ChunkManager(const std::shared_ptr<Terrain> &terrain);

    std::map<ChunkPos, std::unique_ptr<Chunk>> activeChunks;
    std::map<ChunkPos, std::unique_ptr<Chunk>> buildChunks;
    std::vector<std::unique_ptr<Chunk>> recycleList;
    std::shared_ptr<Terrain> terrain = nullptr;
    i32 buildStage = 0;
    std::map<ChunkPos, std::unique_ptr<Chunk>>::iterator setupIterator;
    ChunkPos camChunkPos;
    size_t totalMeshSizeBytes = 0;

    inline Chunk *findChunkAt(const std::map<ChunkPos, std::unique_ptr<Chunk>>&chunk_map, const ChunkPos &pos) {
        auto it = chunk_map.find(pos);
        if(it != chunk_map.end())
            return (*it).second.get();

        return nullptr;
    }

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

    void beginBuild();
    void runIncrementalChunkBuild();
    void update(glm::vec3& campos, BlockTypeDictionary& blockTypeDict);

private:
};


#endif
