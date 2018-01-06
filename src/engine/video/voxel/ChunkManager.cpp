//
// Created by bison on 26-11-2017.
//

#include "ChunkManager.h"

ChunkManager::ChunkManager(const std::shared_ptr<Terrain> &terrain) : terrain(terrain) {}

void ChunkManager::runIncrementalChunkBuild()
{
    if(buildStage == CHUNK_STAGE_SETUP)
    {
        for(int i = 0; i < CHUNKS_SETUP_PER_FRAME; i++)
        {
            if(setupIterator == buildChunks.end())
            {
                buildStage = CHUNK_STAGE_BUILD;
                return;
            }
            else
            {
                setupIterator->second->setupFromTerrain(setupIterator->first, terrain);
            }
            setupIterator++;
        }
    }
    if(buildStage == CHUNK_STAGE_BUILD)
    {
        ChunkNeighbours neighbours;
        //Chunk *search_chunk;
        ChunkPos pos;
        i32 count = 0;
        while(!buildChunks.empty() && count < CHUNKS_BUILD_PER_FRAME)
        {
            auto chunk_it = buildChunks.begin();
            const auto& chunk_pos = chunk_it->first;

            // find north
            pos.set(chunk_pos.x, chunk_pos.z - 1);
            neighbours.n = findBuildChunkAt(pos);
            if(neighbours.n == nullptr)
                neighbours.n = findActiveChunkAt(pos);

            // find south
            pos.set(chunk_pos.x, chunk_pos.z + 1);
            neighbours.s = findBuildChunkAt(pos);
            if(neighbours.s == nullptr)
                neighbours.s = findActiveChunkAt(pos);

            // find west
            pos.set(chunk_pos.x - 1, chunk_pos.z);
            neighbours.w = findBuildChunkAt(pos);
            if(neighbours.w == nullptr)
                neighbours.w = findActiveChunkAt(pos);

            // find east
            pos.set(chunk_pos.x + 1, chunk_pos.z);
            neighbours.e = findBuildChunkAt(pos);
            if(neighbours.e == nullptr)
                neighbours.e = findActiveChunkAt(pos);

            chunk_it->second->rebuild(chunk_it->first, neighbours);
            activeChunks[chunk_it->first] = std::move(chunk_it->second);
            buildChunks.erase(chunk_it);
            count++;
        }
    }
    //SDL_Log("Rebuild %d chunks this frame", count);
}

void ChunkManager::beginBuild() {
    buildStage = CHUNK_STAGE_SETUP;
    setupIterator = buildChunks.begin();
}

void ChunkManager::update(glm::vec3& campos, BlockTypeDictionary& blockTypeDict) {
    worldToChunk(campos, camChunkPos);
    //chunkToWorld(camChunkPos, worldPos);

    bool did_rebuild = false;
    // check if chunks needs to be added, by checking a grid based on the visual radius around the camera
    // calculate width of a box centered on the camera pos
    int size = (VISIBLE_RADIUS * 2) + 1;
    auto m = glm::mat4();

    int chunks_allocated = 0;
    int chunks_recycled = 0;
    int radius = VISIBLE_RADIUS;
    for(int z=-radius; z<=radius; z++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + z * z <= radius * radius)
            {
                ChunkPos testpos;
                glm::vec3 worldpos = {0, 0.0f, 0};
                testpos.x = camChunkPos.x + x;
                testpos.z = camChunkPos.z + z;
                chunkToWorld(testpos, worldpos);
                auto chunk = findActiveChunkAt(testpos);
                auto build_chunk = findBuildChunkAt(testpos);
                if(chunk == nullptr && build_chunk == nullptr)
                {
                    did_rebuild = true;
                    //SDL_Log("No chunk found at pos %d,%d, adding one.", testpos.x, testpos.z);
                    if(!recycleList.empty())
                    {
                        chunks_recycled++;
                        std::unique_ptr<Chunk> nc = std::move(recycleList.back());
                        recycleList.pop_back();
                        nc->position = worldpos;
                        buildChunks[testpos] = std::move(nc);
                    }
                    else {
                        chunks_allocated++;
                        buildChunks[testpos] = std::unique_ptr<Chunk>(new Chunk(blockTypeDict));
                        buildChunks[testpos]->position = worldpos;
                    }
                }
            }
        }
    }

    if(did_rebuild)
    {
        beginBuild();
    }

    if(!buildChunks.empty())
    {
        //auto start = game.getSystem()->getPerformanceCounter();
        runIncrementalChunkBuild();
        //auto end = game.getSystem()->getPerformanceCounter();
        //auto diff = end - start;
        //SDL_Log("Incremental chunk building took %d ms this frame (%d chunks built)", diff / (system.getPerformanceFreq()/1000), CHUNKS_BUILD_PER_FRAME);
    }

    if(did_rebuild)
        SDL_Log("Started rebuilding %d chunks. new = %d, recycled = %d", chunks_recycled+chunks_allocated, chunks_allocated, chunks_recycled);

    totalMeshSizeBytes = 0;
    // check if chunks need to be removed
    for (auto it = activeChunks.begin(); it != activeChunks.end() /* not hoisted */; /* no increment */)
    {
        auto tilepos = (*it).first;
        //auto dist = distance(camChunkPos.x, camChunkPos.z, tilepos.x, tilepos.z);
        auto visible = posInVisibleRadius(tilepos);
        if (!visible)
        {
            recycleList.push_back(std::move((*it).second));
            activeChunks.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            totalMeshSizeBytes += (*it).second->getMeshSizeBytes();
            ++it;
        }
    }

    for(const auto& chunk : recycleList)
    {
        totalMeshSizeBytes += chunk->getMeshSizeBytes();
    }
}

void ChunkManager::worldToChunk(glm::vec3& worldpos, ChunkPos& chunkpos)
{
    auto xoffset = (float) CHUNK_SIZE/2;
    auto zoffset = (float) CHUNK_SIZE/2;
    if(worldpos.x < 0)
        xoffset *= -1;
    if(worldpos.z < 0)
        zoffset *= -1;
    chunkpos.x = (int) ((worldpos.x + xoffset) / (float) CHUNK_SIZE);
    chunkpos.z = (int) ((worldpos.z + zoffset) / (float) CHUNK_SIZE);
}

void ChunkManager::chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos) {
    worldpos.x = (chunkpos.x * CHUNK_SIZE);
    worldpos.z = (chunkpos.z * CHUNK_SIZE);
}
