//
// Created by bison on 26-11-2017.
//

#include <algorithm>
#include "ChunkManager.h"

ChunkManager::ChunkManager(const std::shared_ptr<Terrain> &terrain) : terrain(terrain) {
    lightMapper = std::unique_ptr<LightMapper>(new LightMapper(this, updateList));
    calculateCircleOffsets();
}

void ChunkManager::calculateCircleOffsets()
{
    int radius = VISIBLE_RADIUS;
    // calculate all chunk positions within a circle given the visible radius
    for(int z=-radius; z<=radius; z++) {
        for (int x = -radius; x <= radius; x++) {
            if (sqrt(x * x + z * z) <= radius) {
                circlePosOffsets.emplace_back(x,z);
            }
        }
    }
    SDL_Log("Calculated %d circular offsets", circlePosOffsets.size());
    // sort them according to distance
    std::sort(circlePosOffsets.begin(), circlePosOffsets.end(),
         [](const ChunkPos& a, const ChunkPos& b)
         {
             auto dist_a = sqrt(a.x*a.x + a.z*a.z);
             auto dist_b = sqrt(b.x*b.x + b.z*b.z);
             return dist_a < dist_b;
         });
    auto i = 0;
    for(auto &pos : circlePosOffsets)
    {
        //SDL_Log("Offset[%d]: %d,%d", i, pos.x, pos.z);
        i++;
    }
    activeChunks.reserve(circlePosOffsets.size());
    buildChunks.reserve(circlePosOffsets.size());
}

void ChunkManager::runIncrementalChunkBuild()
{
    if(buildStage == CHUNK_STAGE_SETUP)
    {
        for(int i = 0; i < CHUNKS_SETUP_PER_FRAME; i++)
        {
            if(setupIterator == buildChunks.cend())
            {
                SDL_Log("CHUNK_STAGE_LIGHTING");

                buildStage = CHUNK_STAGE_LIGHTING;
                currentLightPosIndex = circlePosOffsets.size()-1;
                return;
            }
            else
            {
                if(setupIterator->first.x == 0 && setupIterator->first.z == 0
                   || setupIterator->first.x == 1 && setupIterator->first.z == 0
                    || setupIterator->first.x == 0 && setupIterator->first.z == 1
                       || setupIterator->first.x == 1 && setupIterator->first.z == 1)
                    setupIterator->second->setupDebugChunk();
                else
                    setupIterator->second->setupFromTerrain(setupIterator->first, terrain);
            }
            setupIterator++;
        }
    }
    if(buildStage == CHUNK_STAGE_LIGHTING)
    {
        ChunkPos *positions = circlePosOffsets.data();
        ChunkPos testpos;
        for(int i = 0; i < CHUNKS_LIT_PER_FRAME; i++)
        {
            if(currentLightPosIndex < 0)
            {
                SDL_Log("CHUNK_STAGE_BUILD");
                buildStage = CHUNK_STAGE_BUILD;
                return;
            }
            else
            {
                testpos.x = camChunkPos.x + positions[currentLightPosIndex].x;
                testpos.z = camChunkPos.z + positions[currentLightPosIndex].z;
                Chunk *chunk = findBuildChunkAt(testpos);
                if(chunk != nullptr) {
                    SDL_Log("Lighting chunk %d,%d", testpos.x, testpos.z);
                    chunk->clearLightMap();
                    lightMapper->calculateSunlight(chunk);
                    //lightMapper->clipping = false;
                }
            }
            currentLightPosIndex--;
        }
    }

    if(buildStage == CHUNK_STAGE_BUILD)
    {
        i32 count = 0;
        while(!buildChunks.empty() && count < CHUNKS_BUILD_PER_FRAME)
        {
            auto chunk_it = buildChunks.begin();
            const auto& chunk_pos = chunk_it->first;

            //SDL_Log("chk.xy = %d,%d n=%d s=%d w=%d e=%d", chunk_pos.x, chunk_pos.z, all_neighbours.n, all_neighbours.s, all_neighbours.w, all_neighbours.e);

            //chunk_it->second->clearLightMap();
            //lightMapper->calculateSunlight(chunk_it->second.get());

            chunk_it->second->rebuild(chunk_it->first);

            activeChunks[chunk_it->first] = std::move(chunk_it->second);
            buildChunks.erase(chunk_it);
            count++;
        }
        if(buildChunks.empty())
        {
            SDL_Log("BUILD stage completed");
            //SDL_Log("activeChunks load factor: %.2f", activeChunks.load_factor());
            if(!optimizeList.empty())
            {
                SDL_Log("CHUNK_STAGE_OPTIMIZE");
                buildStage = CHUNK_STAGE_OPTIMIZE;
            }
            else {
                SDL_Log("CHUNK_STAGE_IDLE");
                buildStage = CHUNK_STAGE_IDLE;
            }
            return;
        }
    }

    if(buildStage == CHUNK_STAGE_OPTIMIZE)
    {
        i32 count = 0;
        while(!optimizeList.empty() && count < CHUNKS_BUILD_PER_FRAME) {
            Chunk* chunk = optimizeList.back();
            optimizeList.pop_back();
            //SDL_Log("optimizeList chunk pos %d,%d", chunk->chunkPosition.x, chunk->chunkPosition.z);
            chunk->clearLightMap();
            lightMapper->calculateSunlight(chunk);
            chunk->rebuild(chunk->chunkPosition);
            count++;
        }
        //SDL_Log("Remeshed %d chunks this frame", count);
        if(optimizeList.empty())
        {
            SDL_Log("CHUNK_STAGE_IDLE");
            buildStage = CHUNK_STAGE_IDLE;
        }
    }

    if(buildStage == CHUNK_STAGE_UPDATE)
    {
        i32 count = 0;
        while(!updateList.empty() && count < CHUNKS_UPDATED_PER_FRAME) {
            Chunk* chunk = updateList.back();
            updateList.pop_back();
            //SDL_Log("optimizeList chunk pos %d,%d", chunk->chunkPosition.x, chunk->chunkPosition.z);
            chunk->rebuild(chunk->chunkPosition);
            count++;
        }
        SDL_Log("Updated %d chunks this frame", count);
        if(updateList.empty())
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

    //bool created_any = false;
    ChunkPos testpos;
    glm::vec3 worldpos = {0, 0.0f, 0};

    ChunkPos *positions = circlePosOffsets.data();
    for(i32 i = 0; i < circlePosOffsets.size(); i++)
    {
        testpos.x = camChunkPos.x + positions[i].x;
        testpos.z = camChunkPos.z + positions[i].z;
        chunkToWorld(testpos, worldpos);
        auto chunk = findActiveChunkAt(testpos);
        auto build_chunk = findBuildChunkAt(testpos);
        if(chunk == nullptr && build_chunk == nullptr)
        {
            //created_any = true;
            //SDL_Log("No chunk found at pos %d,%d, adding one.", testpos.x, testpos.z);
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
                buildChunks[testpos] = std::unique_ptr<Chunk>(new Chunk(blockTypeDict, this));
                buildChunks[testpos]->position = worldpos;
                buildChunks[testpos]->chunkPosition = testpos;
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
    ChunkPos pos;
    Chunk *active_chunk;
    for(const auto& chunk : buildChunks)
    {
        //findNeighbours(activeChunks, neighbours, chunk.first);
        // north
        pos.set(chunk.first.x, chunk.first.z-1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // south
        pos.set(chunk.first.x, chunk.first.z+1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // west
        pos.set(chunk.first.x-1, chunk.first.z);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // east
        pos.set(chunk.first.x+1, chunk.first.z);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

    }
    // delete duplicates
    std::sort(optimizeList.begin(), optimizeList.end());
    optimizeList.erase( std::unique(optimizeList.begin(), optimizeList.end()), optimizeList.end());
    SDL_Log("Added %d old neighbours from active list to rebuild list", optimizeList.size());
}

void ChunkManager::update(glm::vec3& campos, BlockTypeDictionary& blockTypeDict) {
    // Update camera's position in chunk coords
    worldToChunk(campos, camChunkPos);
    worldToBlock(campos, camBlockWorldPos);

    worldPosToChunkBlockPos(camBlockWorldPos.x, camBlockWorldPos.y, camBlockWorldPos.z, camBlockLocalPos);

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
            else
            {
                // add all chunks to rebuild
                for(const auto& chunk : buildChunks)
                {
                    optimizeList.push_back(chunk.second.get());
                }
                SDL_Log("Added new chunks to rebuild list %d", optimizeList.size());
            }
            SDL_Log("CHUNK_STAGE_SETUP");
            buildStage = CHUNK_STAGE_SETUP;
            setupIterator = buildChunks.begin();
            return;
        }
        if(!updateList.empty())
        {
            SDL_Log("CHUNK_STAGE_UPDATE");
            buildStage = CHUNK_STAGE_UPDATE;
            return;
        }
    }

    runIncrementalChunkBuild();
}

void ChunkManager::worldToBlock(glm::vec3& worldpos, ChunkBlockPos& cbpos)
{
    auto xoffset = halfChunkSize;
    auto zoffset = halfChunkSize;

    if(worldpos.x < 0) {
        cbpos.x = (int) floor((worldpos.x + xoffset));
    }
    else
    {
        cbpos.x = (int) (worldpos.x + xoffset);
    }
    if(worldpos.z < 0) {
        cbpos.z = (int) floor((worldpos.z + zoffset));
    }
    else
    {
        cbpos.z = (int) (worldpos.z + zoffset);
    }
    cbpos.y = (int) (worldpos.y);
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




// does a BFS search (brett first search ;)
void ChunkManager::placeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z, u8 level) {
    lightMapper->placeTorchLight(origin_chunk, x, y, z, level);
}

void ChunkManager::removeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z) {
    lightMapper->removeTorchLight(origin_chunk, x, y, z);
}


void ChunkManager::testStuff()
{
    SDL_Log("Chunkman test stuff:");
    ChunkBlockPos cbpos;
    //Chunk *chunk = findActiveChunkAt(ChunkPos(0,0));
    //if(chunk != nullptr) {
        for(auto i = 0; i < 35; i++) {
           worldPosToChunkBlockPos(-i, 8, 0, cbpos);
           //SDL_Log("Transformed block %d,%d,%d (chunk: %d,%d) to %d,%d,%d (chunk: %d,%d)", i, 8, 0, chunk->chunkPosition.x, chunk->chunkPosition.z, cbpos.x, cbpos.y, cbpos.z, cbpos.chunk->chunkPosition.x, cbpos.chunk->chunkPosition.z);
            if(cbpos.chunk != nullptr)
                SDL_Log("Transformed block world pos %d,%d,%d to %d,%d,%d (chunk: %d,%d)", -i, 8, 0, cbpos.x, cbpos.y, cbpos.z, cbpos.chunk->chunkPosition.x, cbpos.chunk->chunkPosition.z);
            else
                SDL_Log("Could not find chunk for %d,%d,%d", cbpos.x, cbpos.y, cbpos.z);
        }
    //}
    //else
    //    SDL_Log("Could not find origin chunk");
}
