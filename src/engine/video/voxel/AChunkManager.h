//
// Created by bison on 26-01-2018.
//

#ifndef GAME_ACHUNKMANAGER_H
#define GAME_ACHUNKMANAGER_H

#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include "Chunk.h"
#include "ChunkBlockPos.h"
#include "IChunkManager.h"
#include "LightMapper.h"
#include "ChunkMetrics.h"
#include <engine/util/BlockingQueue.h>
#include <thread>
#include <atomic>
#include <mutex>

#define JOB_STATUS_IDLE 0
#define JOB_STATUS_SETUP 1
#define JOB_STATUS_LIGHTING 2
#define JOB_STATUS_MESHING 3
#define JOB_STATUS_OPTIMIZING 4
#define JOB_STATUS_UPDATING 5

typedef enum {BUILD, PLACE_TORCH, REMOVE_TORCH, QUIT} JobType;

struct JobRequest {
    JobType type;
    ChunkPos chunkPos;
    Chunk *chunk;
    i32 x, y, z, level;
    JobRequest(JobType type) : type(type) {}
};

class AChunkManager : public IChunkManager {
public:
    const int VISIBLE_RADIUS = 32;

    AChunkManager(IGame &game, BlockTypeDictionary &blockTypeDict, const std::shared_ptr<Terrain> &terrain);
    virtual ~AChunkManager();
    void runThread();
    void update(glm::vec3& campos);

    void worldToChunk(glm::vec3 &worldpos, ChunkPos &chunkpos);
    void chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos);
    void worldToBlock(glm::vec3 &worldpos, ChunkBlockPos &cbpos);

    inline bool posInVisibleRadius(ChunkPos& pos)
    {
        int dx = pos.x - camChunkPos.x;
        int dy = pos.z - camChunkPos.z;
        return sqrt(dx*dx + dy*dy) <= VISIBLE_RADIUS;
    }

    inline Chunk *findActiveChunkAt(const ChunkPos &pos) {
        return findChunkAt(activeChunks, pos);
    }

    inline Chunk *findBuildChunkAt(const ChunkPos &pos) {
        return findChunkAt(buildChunks, pos);
    }

    inline Chunk *findChunkAt(const ChunkPos &pos) {
        auto c = findActiveChunkAt(pos);
        if(c == nullptr)
            c = findBuildChunkAt(pos);
        return c;
    }

    inline void relativePosToChunkBlockPos(Chunk *chunk, i32 x, i32 y, i32 z, ChunkBlockPos& chunkBlockPos)
    {
        // early out reducing this to a two dimensional problem
        if(y < 0 || y >= CHUNK_HEIGHT)
        {
            //SDL_Log("%d,%d,%d Y coordinate is outside range, returning invalid pos", x, y, z);
            chunkBlockPos.setInvalid();
            return;
        }
        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            //SDL_Log("%d,%d,%d Y coordinate is within same chunk", x, y, z);
            chunkBlockPos.chunk = chunk;
            chunkBlockPos.x = x;
            chunkBlockPos.y = y;
            chunkBlockPos.z = z;
            return;
        }

        i32 chk_off_x, l_x;
        if(x >= 0) {
            chk_off_x = x / CHUNK_SIZE;
            l_x = x % CHUNK_SIZE;
        }
        else // subtract -1 because the first position was in the original chunk
        {
            auto abs_x = abs(x)-1;
            chk_off_x = -1 * (((abs_x + CHUNK_SIZE) / CHUNK_SIZE));
            l_x = (abs_x % CHUNK_SIZE);
            l_x = CHUNK_SIZE - l_x - 1;
        }

        i32 chk_off_z, l_z;
        if(z >= 0) {
            chk_off_z = z / CHUNK_SIZE;
            l_z = z % CHUNK_SIZE;
        }
        else // subtract -1 because the first position was in the original chunk
        {
            i32 abs_z = abs(z) - 1;
            chk_off_z = -1 * (((abs_z + CHUNK_SIZE) / CHUNK_SIZE));
            l_z = (abs_z % CHUNK_SIZE);
            l_z = CHUNK_SIZE - l_z - 1;
        }
        ChunkPos c_pos = {chunk->chunkPosition.x + chk_off_x, chunk->chunkPosition.z + chk_off_z};
        //SDL_Log("New chunk pos %d,%d", c_pos.x, c_pos.z);
        chunkBlockPos.chunk = findChunkAt(c_pos);
        if(chunkBlockPos.chunk == nullptr)
        {
            //SDL_Log("Chunk could not be found at pos %d,%d,%d, returning invalid", x, y, z);
            chunkBlockPos.setInvalid();
            return;
        }
        chunkBlockPos.x = l_x;
        chunkBlockPos.y = y;
        chunkBlockPos.z = l_z;
        //SDL_Log("Transformed block %d,%d,%d (chunk: %d,%d) to %d,%d,%d (chunk: %d,%d)", x, y, z, chunk->chunkPosition.x, chunk->chunkPosition.z, chunkBlockPos.x, chunkBlockPos.y, chunkBlockPos.z, c_pos.x, c_pos.z);
    }

    inline void worldPosToChunkBlockPos(i32 x, i32 y, i32 z, ChunkBlockPos& chunkBlockPos)
    {
        // within the same chunk
        if(x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
        {
            //SDL_Log("%d,%d,%d Y coordinate is within same chunk", x, y, z);
            chunkBlockPos.chunk = findActiveChunkAt(ChunkPos(0,0));
            chunkBlockPos.x = x;
            chunkBlockPos.y = y;
            chunkBlockPos.z = z;
            return;
        }

        i32 chk_off_x, l_x;
        if(x >= 0) {
            chk_off_x = x / CHUNK_SIZE;
            l_x = x % CHUNK_SIZE;
        }
        else // subtract -1 because the first position was in the original chunk
        {
            auto abs_x = abs(x)-1;
            chk_off_x = -1 * (((abs_x + CHUNK_SIZE) / CHUNK_SIZE));
            l_x = (abs_x % CHUNK_SIZE);
            l_x = CHUNK_SIZE - l_x - 1;
        }

        i32 chk_off_z, l_z;
        if(z >= 0) {
            chk_off_z = z / CHUNK_SIZE;
            l_z = z % CHUNK_SIZE;
        }
        else // subtract -1 because the first position was in the original chunk
        {
            i32 abs_z = abs(z) - 1;
            chk_off_z = -1 * (((abs_z + CHUNK_SIZE) / CHUNK_SIZE));
            l_z = (abs_z % CHUNK_SIZE);
            l_z = CHUNK_SIZE - l_z - 1;
        }
        ChunkPos c_pos = {chk_off_x, chk_off_z};
        //SDL_Log("New chunk pos %d,%d", c_pos.x, c_pos.z);
        chunkBlockPos.chunk = findChunkAt(c_pos);
        if(chunkBlockPos.chunk == nullptr)
        {
            SDL_Log("Chunk could not be found at that pos, returning invalid");
            chunkBlockPos.setInvalid();
            return;
        }
        chunkBlockPos.x = l_x;
        chunkBlockPos.y = y;
        chunkBlockPos.z = l_z;
        //SDL_Log("Transformed block %d,%d,%d (chunk: %d,%d) to %d,%d,%d (chunk: %d,%d)", x, y, z, chunk->chunkPosition.x, chunk->chunkPosition.z, chunkBlockPos.x, chunkBlockPos.y, chunkBlockPos.z, c_pos.x, c_pos.z);
    }

    inline std::string getBuildStageAsString() {
        switch(status)
        {
            case JOB_STATUS_IDLE:
                return "IDLE";
            case JOB_STATUS_SETUP:
                return "SETUP";
            case JOB_STATUS_MESHING:
                return "MESHING";
            case JOB_STATUS_OPTIMIZING:
                return "OPTIMIZING";
            case JOB_STATUS_UPDATING:
                return "UPDATING";
            case JOB_STATUS_LIGHTING:
                return "LIGHTING";
            default:
                return "UNKNOWN";
        }
    }

    void placeTorchLight(Chunk *origin_chunk, int x, int y, int z, u8 level);
    void removeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z);
    i32 getActiveChunkCount();
    void logChunkMetrics();

    ChunkMetrics chunkMetrics;
    std::vector<ChunkPos> circlePosOffsets;
    ChunkPos camChunkPos;
    std::mutex activeLock;
    ChunkBlockPos camBlockWorldPos;
    ChunkBlockPos camBlockLocalPos;
    size_t totalMeshSizeBytes = 0;
    float sunlightIntensity = 0;

private:
    std::thread thread;
    BlockingQueue<JobRequest> requestQueue;
    IGame& game;
    BlockTypeDictionary& blockTypeDict;
    std::unique_ptr<LightMapper> lightMapper;
    float halfChunkSize = (float) CHUNK_SIZE/2;
    float fChunkSize = (float) CHUNK_SIZE;
    std::shared_ptr<Terrain> terrain = nullptr;
    std::atomic<i32> status;
    std::atomic<i32> activeChunkCount;

    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> activeChunks;
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> buildChunks;
    std::vector<std::unique_ptr<Chunk>> recycleList;
    std::vector<Chunk*> optimizeList;
    std::vector<Chunk*> updateList;

    ChunkPos lastCamChunkPos;
    bool firstRun = true;
    bool firstUpdate = true;


    void calculateCircleOffsets();

    inline Chunk *findChunkAt(const std::unordered_map<ChunkPos, std::unique_ptr<Chunk>>&chunk_map, const ChunkPos &pos) {
        std::unordered_map<ChunkPos, std::unique_ptr<Chunk>>::const_iterator it = chunk_map.find(pos);
        if(it != chunk_map.cend())
            return (*it).second.get();

        return nullptr;
    }

    void buildJob();
    void recycleChunks();
    void createChunks();
    void determineChunksToRebuild();
    void setupChunks();
    void lightChunks();
    void meshChunks();
    void optimizeChunks();
    void updateChunks();

    double startWatchTime;
    double lastWatchResult;

    inline void startWatch()
    {
        startWatchTime = game.getTime();
    }

    inline void stopWatch()
    {
        lastWatchResult = game.getTime() - startWatchTime;
    }
};


#endif //GAME_ACHUNKMANAGER_H
