//
// Created by bison on 26-11-2017.
//

#include <algorithm>
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
                SDL_Log("CHUNK_STAGE_BUILD");
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
        i32 count = 0;
        while(!buildChunks.empty() && count < CHUNKS_BUILD_PER_FRAME)
        {
            auto chunk_it = buildChunks.begin();
            const auto& chunk_pos = chunk_it->first;

            findNeighbours(neighbours, chunk_pos);
            //SDL_Log("chk.xy = %d,%d n=%d s=%d w=%d e=%d", chunk_pos.x, chunk_pos.z, all_neighbours.n, all_neighbours.s, all_neighbours.w, all_neighbours.e);

            chunk_it->second->rebuild(chunk_it->first, neighbours);
            activeChunks[chunk_it->first] = std::move(chunk_it->second);
            buildChunks.erase(chunk_it);
            count++;
        }
        if(buildChunks.empty())
        {
            SDL_Log("BUILD stage completed");
            if(!rebuildList.empty())
            {
                SDL_Log("CHUNK_STAGE_REBUILD");
                buildStage = CHUNK_STAGE_REBUILD;
            }
            else {
                SDL_Log("CHUNK_STAGE_IDLE");
                buildStage = CHUNK_STAGE_IDLE;
            }
            return;
        }
    }

    if(buildStage == CHUNK_STAGE_REBUILD)
    {
        ChunkNeighbours neighbours;
        i32 count = 0;
        while(!rebuildList.empty() && count < CHUNKS_BUILD_PER_FRAME) {
            Chunk* chunk = rebuildList.back();
            rebuildList.pop_back();
            //SDL_Log("rebuildList chunk pos %d,%d", chunk->chunkPosition.x, chunk->chunkPosition.z);
            findNeighbours(neighbours, chunk->chunkPosition);
            chunk->rebuild(chunk->chunkPosition, neighbours);
            count++;
        }
        //SDL_Log("Remeshed %d chunks this frame", count);
        if(rebuildList.empty())
        {
            SDL_Log("CHUNK_STAGE_IDLE");
            buildStage = CHUNK_STAGE_IDLE;
        }
    }
}

void ChunkManager::createChunks(BlockTypeDictionary& blockTypeDict)
{
    auto m = glm::mat4();
    int chunks_allocated = 0;
    int chunks_recycled = 0;
    int radius = VISIBLE_RADIUS;
    // brute force a rough circle (no sqrt)
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
                    //SDL_Log("No chunk found at pos %d,%d, adding one.", testpos.x, testpos.z);
                    /*
                    if(buildStage != CHUNK_STAGE_IDLE) {
                        SDL_Log("Adding chunks to build list while not in state idle");
                        exit(-2);
                    }
                     */
                    if(!recycleList.empty())
                    {
                        chunks_recycled++;
                        std::unique_ptr<Chunk> nc = std::move(recycleList.back());
                        recycleList.pop_back();
                        nc->position = worldpos;
                        nc->chunkPosition = testpos;
                        buildChunks[testpos] = std::move(nc);
                    }
                    else {
                        chunks_allocated++;
                        buildChunks[testpos] = std::unique_ptr<Chunk>(new Chunk(blockTypeDict));
                        buildChunks[testpos]->position = worldpos;
                        buildChunks[testpos]->chunkPosition = testpos;
                    }
                }
            }
        }
    }
}

void ChunkManager::recycleChunks()
{
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

void ChunkManager::determineChunksToRebuild()
{
    ChunkNeighbours neighbours;
    for(const auto& chunk : buildChunks)
    {
        findNeighbours(activeChunks, neighbours, chunk.first);
        //SDL_Log("Build chunk xy = %d,%d has neighbours n=%d s=%d w=%d e=%d", chunk.first.x, chunk.first.z, neighbours.n, neighbours.s, neighbours.w, neighbours.e);
        if(neighbours.n != nullptr)
            rebuildList.push_back(neighbours.n);
        if(neighbours.s != nullptr)
            rebuildList.push_back(neighbours.s);
        if(neighbours.w != nullptr)
            rebuildList.push_back(neighbours.w);
        if(neighbours.e != nullptr)
            rebuildList.push_back(neighbours.e);
    }
    // delete duplicates
    std::sort(rebuildList.begin(), rebuildList.end());
    rebuildList.erase( std::unique(rebuildList.begin(), rebuildList.end()), rebuildList.end());
    SDL_Log("Added %d old neighbours from active list to rebuild list", rebuildList.size());
}

void ChunkManager::update(glm::vec3& campos, BlockTypeDictionary& blockTypeDict) {
    // Update camera's position in chunk coords
    worldToChunk(campos, camChunkPos);
    if(buildStage == CHUNK_STAGE_IDLE)
    {
        // recycle chunks out of view
        if(!activeChunks.empty())
            recycleChunks();
        // scan for new chunks needed and start building if any are found
        createChunks(blockTypeDict);
        if(!buildChunks.empty()) {
            if(!activeChunks.empty())   // if not first build, mark fringe neighbours for rebuild
            {
                determineChunksToRebuild();
            }
            SDL_Log("CHUNK_STAGE_SETUP");
            buildStage = CHUNK_STAGE_SETUP;
            setupIterator = buildChunks.begin();
        }
    }

    runIncrementalChunkBuild();

}

void ChunkManager::worldToChunk(glm::vec3& worldpos, ChunkPos& chunkpos)
{
    auto xoffset = halfChunkSize;
    auto zoffset = halfChunkSize;
    if(worldpos.x < 0)
        xoffset *= -1;
    if(worldpos.z < 0)
        zoffset *= -1;
    chunkpos.x = (int) ((worldpos.x + xoffset) / fChunkSize);
    chunkpos.z = (int) ((worldpos.z + zoffset) / fChunkSize);
}

void ChunkManager::chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos) {
    worldpos.x = (chunkpos.x * CHUNK_SIZE);
    worldpos.z = (chunkpos.z * CHUNK_SIZE);
    worldpos.x -= halfChunkSize;
    worldpos.z -= halfChunkSize;
}

ChunkPos ChunkManager::getChunkFromWorld(glm::vec3 &worldpos)
{
    ChunkPos pos;
    worldToChunk(worldpos, pos);
    return pos;
}

void ChunkManager::findNeighbours(ChunkNeighbours& neighbours, const ChunkPos& chunk_pos)
{
    ChunkPos pos;
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
}

void ChunkManager::findNeighbours(const std::map<ChunkPos, std::unique_ptr<Chunk>> &chunk_map, ChunkNeighbours &neighbours,
                             const ChunkPos &chunk_pos) {
    ChunkPos pos;
    // find north
    pos.set(chunk_pos.x, chunk_pos.z - 1);
    neighbours.n = findChunkAt(chunk_map, pos);

    // find south
    pos.set(chunk_pos.x, chunk_pos.z + 1);
    neighbours.s = findChunkAt(chunk_map, pos);

    // find west
    pos.set(chunk_pos.x - 1, chunk_pos.z);
    neighbours.w = findChunkAt(chunk_map, pos);

    // find east
    pos.set(chunk_pos.x + 1, chunk_pos.z);
    neighbours.e = findChunkAt(chunk_map, pos);
}
