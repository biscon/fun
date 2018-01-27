//
// Created by bison on 26-01-2018.
//

#include <algorithm>
#include "AChunkManager.h"
#include "ChunkManager.h"

AChunkManager::AChunkManager(IGame &game, BlockTypeDictionary &blockTypeDict, const std::shared_ptr<Terrain> &terrain)
        : game(game), blockTypeDict(blockTypeDict), terrain(terrain)
{
    lightMapper = std::unique_ptr<LightMapper>(new LightMapper(this, updateList));
    calculateCircleOffsets();
    thread = std::thread(&AChunkManager::runThread, this);
    status = JOB_STATUS_IDLE;
    activeChunkCount = 0;
}

AChunkManager::~AChunkManager() {
    requestQueue.push({JobType::QUIT});
    thread.join();
}

void AChunkManager::calculateCircleOffsets()
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

// does a BFS search (brett first search ;)
void AChunkManager::placeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z, u8 level) {
    JobRequest request(JobType::PLACE_TORCH);
    request.x = x;
    request.y = y;
    request.z = z;
    request.level = level;
    request.chunk = origin_chunk;
    requestQueue.push(request);
}

void AChunkManager::removeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z) {
    JobRequest request(JobType::REMOVE_TORCH);
    request.x = x;
    request.y = y;
    request.z = z;
    request.level = 0;
    request.chunk = origin_chunk;
    requestQueue.push(request);
}

void AChunkManager::worldToChunk(glm::vec3& worldpos, ChunkPos& chunkpos)
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

void AChunkManager::chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos) {
    worldpos.x = (chunkpos.x * CHUNK_SIZE);
    worldpos.z = (chunkpos.z * CHUNK_SIZE);
    worldpos.x -= halfChunkSize;
    worldpos.z -= halfChunkSize;
}

void AChunkManager::recycleChunks()
{
    //std::lock_guard<std::mutex> lock(activeLock);
    totalMeshSizeBytes = 0;
    // check if chunks need to be removed
    for (auto it = activeChunks.begin(); it != activeChunks.end() /* not hoisted */; /* no increment */)
    {
        auto tilepos = (*it).first;
        //auto dist = distance(camChunkPos.x, camChunkPos.z, tilepos.x, tilepos.z);
        auto visible = posInVisibleRadius(tilepos);
        if (!visible)
        {
            std::unique_lock<std::mutex> lock(activeLock);
            recycleList.push_back(std::move((*it).second));
            activeChunks.erase(it++);    // or "it = m.erase(it)" since C++11
            lock.unlock();
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

void AChunkManager::worldToBlock(glm::vec3& worldpos, ChunkBlockPos& cbpos)
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

void AChunkManager::createChunks()
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
    SDL_Log("Created %d chunks, recycled %d", chunks_allocated, chunks_recycled);
}

void AChunkManager::determineChunksToRebuild()
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

        // northwest
        pos.set(chunk.first.x-1, chunk.first.z-1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // southwest
        pos.set(chunk.first.x-1, chunk.first.z+1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // northeast
        pos.set(chunk.first.x+1, chunk.first.z-1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

        // southeast
        pos.set(chunk.first.x+1, chunk.first.z+1);
        active_chunk = findActiveChunkAt(pos);
        if(active_chunk != nullptr)
            optimizeList.push_back(active_chunk);

    }
    // delete duplicates
    std::sort(optimizeList.begin(), optimizeList.end());
    optimizeList.erase( std::unique(optimizeList.begin(), optimizeList.end()), optimizeList.end());
    SDL_Log("Added %d old neighbours from active list to rebuild list", optimizeList.size());
}

void AChunkManager::update(glm::vec3 &campos) {
    // Update camera's position in chunk coords if moved
    ChunkPos new_pos;
    worldToChunk(campos, new_pos);
    worldToBlock(campos, camBlockWorldPos);
    worldPosToChunkBlockPos(camBlockWorldPos.x, camBlockWorldPos.y, camBlockWorldPos.z, camBlockLocalPos);

    if(new_pos.x != lastCamChunkPos.x || new_pos.z != lastCamChunkPos.z || firstUpdate)
    {
        firstUpdate = false;
        lastCamChunkPos.x = new_pos.x;
        lastCamChunkPos.z = new_pos.z;
        JobRequest request(JobType::BUILD);
        request.chunkPos = new_pos;
        requestQueue.push(request);
    }
    //uploadChunks();
}

void AChunkManager::setupChunks()
{
    SDL_Log("CHUNK_STAGE_SETUP");
    status = JOB_STATUS_SETUP;
    startWatch();
    std::unordered_map<ChunkPos, std::unique_ptr<Chunk>>::const_iterator setupIterator;
    setupIterator = buildChunks.cbegin();
    while(setupIterator != buildChunks.cend())
    {
        if(setupIterator->first.x == 0 && setupIterator->first.z == 0
           || setupIterator->first.x == 1 && setupIterator->first.z == 0
           || setupIterator->first.x == 0 && setupIterator->first.z == 1
           || setupIterator->first.x == 1 && setupIterator->first.z == 1)
            setupIterator->second->setupDebugChunk();
        else
            setupIterator->second->setupFromTerrain(setupIterator->first, terrain);
        setupIterator++;
    }
    stopWatch();
    chunkMetrics.chunkSetupTime = lastWatchResult;
}

void AChunkManager::lightChunks()
{
    SDL_Log("CHUNK_STAGE_LIGHTING");
    status = JOB_STATUS_LIGHTING;
    startWatch();
    i32 currentLightPosIndex = circlePosOffsets.size()-1;
    ChunkPos *positions = circlePosOffsets.data();
    ChunkPos testpos;
    while(currentLightPosIndex >= 0)
    {
        testpos.x = camChunkPos.x + positions[currentLightPosIndex].x;
        testpos.z = camChunkPos.z + positions[currentLightPosIndex].z;
        Chunk *chunk = findBuildChunkAt(testpos);
        if(chunk != nullptr) {
            //SDL_Log("Lighting chunk %d,%d", testpos.x, testpos.z);
            chunk->clearLightMap();
            lightMapper->calculateSunlight(chunk);
        }
        currentLightPosIndex--;
    }
    stopWatch();
    chunkMetrics.chunkLightingTime = lastWatchResult;
}

void AChunkManager::meshChunks()
{
    SDL_Log("CHUNK_STAGE_MESHING");
    status = JOB_STATUS_MESHING;
    startWatch();
    //std::lock_guard<std::mutex> lock(activeLock);
    while(!buildChunks.empty())
    {
        auto chunk_it = buildChunks.begin();
        const auto& chunk_pos = chunk_it->first;
        chunk_it->second->rebuild(chunk_it->first);
        std::unique_lock<std::mutex> lock(activeLock);
        activeChunks[chunk_it->first] = std::move(chunk_it->second);
        lock.unlock();
        buildChunks.erase(chunk_it);
    }
    stopWatch();
    chunkMetrics.chunkMeshingTime = lastWatchResult;
    activeChunkCount = activeChunks.size();
}

void AChunkManager::optimizeChunks() {
    SDL_Log("CHUNK_STAGE_OPTIMIZE");
    status = JOB_STATUS_OPTIMIZING;
    startWatch();
    //std::lock_guard<std::mutex> lock(activeLock);
    while(!optimizeList.empty())
    {
        Chunk* chunk = optimizeList.back();
        optimizeList.pop_back();
        //SDL_Log("optimizeList chunk pos %d,%d", chunk->chunkPosition.x, chunk->chunkPosition.z);
        chunk->clearLightMap();
        lightMapper->calculateSunlight(chunk);
        std::unique_lock<std::mutex> lock(activeLock);
        chunk->rebuild(chunk->chunkPosition);
        lock.unlock();
    }
    stopWatch();
    chunkMetrics.chunkOptimizeTime = lastWatchResult;
}

void AChunkManager::updateChunks() {
    SDL_Log("CHUNK_STAGE_UPDATE");
    status = JOB_STATUS_UPDATING;
    startWatch();
    //std::lock_guard<std::mutex> lock(activeLock);
    while(!updateList.empty()) {
        Chunk* chunk = updateList.back();
        updateList.pop_back();
        //SDL_Log("optimizeList chunk pos %d,%d", chunk->chunkPosition.x, chunk->chunkPosition.z);
        std::unique_lock<std::mutex> lock(activeLock);
        chunk->rebuild(chunk->chunkPosition);
        lock.unlock();
    }
    stopWatch();
    chunkMetrics.chunkUpdateTime = lastWatchResult;
}

void AChunkManager::buildJob()
{
    //std::unique_lock<std::mutex> lock(activeLock);
    if(!activeChunks.empty()) {
        recycleChunks();
        firstRun = false;
    }
    else
        firstRun = true;

    createChunks();
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
    }
    else {
        SDL_Log("No new chunks created, going back to idle");
        return;
    }

    setupChunks();
    lightChunks();
    meshChunks();
    optimizeChunks();
}

void AChunkManager::runThread() {
    SDL_Log("Chunk thread starting");
    while(true)
    {
        status = JOB_STATUS_IDLE;
        JobRequest job = requestQueue.pop();
        if(job.type == JobType::QUIT)
            break;
        if(job.type == JobType::BUILD)
        {
            SDL_Log("New build job request center = %d,%d", job.chunkPos.x, job.chunkPos.z);
            camChunkPos = job.chunkPos;
            buildJob();
            SDL_Log("Finished build job request");
        }
        if(job.type == JobType::PLACE_TORCH)
        {
            lightMapper->placeTorchLight(job.chunk, job.x, job.y, job.z, job.level);
            updateChunks();
        }
        if(job.type == JobType::REMOVE_TORCH)
        {
            lightMapper->removeTorchLight(job.chunk, job.x, job.y, job.z);
            updateChunks();
        }
    }
    SDL_Log("Chunk thread exiting");
}

void AChunkManager::logChunkMetrics() {
    SDL_Log("-=* ChunkMetrics *=----------------------------");
    SDL_Log("SetupTime    : %0.5f sec (%.2f ms)", chunkMetrics.chunkSetupTime, chunkMetrics.chunkSetupTime * 1000);
    SDL_Log("LightingTime : %0.5f sec (%.2f ms)", chunkMetrics.chunkLightingTime, chunkMetrics.chunkLightingTime * 1000);
    SDL_Log("MeshingTime  : %0.5f sec (%.2f ms)", chunkMetrics.chunkMeshingTime, chunkMetrics.chunkMeshingTime * 1000);
    SDL_Log("OptimizeTime : %0.5f sec (%.2f ms)", chunkMetrics.chunkOptimizeTime, chunkMetrics.chunkOptimizeTime * 1000);
    SDL_Log("UpdateTime   : %0.5f sec (%.2f ms)", chunkMetrics.chunkUpdateTime, chunkMetrics.chunkUpdateTime * 1000);
    SDL_Log("-----------------------------------------------");
}

i32 AChunkManager::getActiveChunkCount() {
    return activeChunkCount;
}
