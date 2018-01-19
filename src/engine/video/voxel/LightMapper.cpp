//
// Created by bison on 19-01-2018.
//

#include <algorithm>
#include "LightMapper.h"

LightMapper::LightMapper(IChunkManager *chunkManager, std::vector<Chunk *> &updateList) : chunkManager(chunkManager),
                                                                                          updateList(updateList) {}

void LightMapper::propagateTorchLight(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel)
{
    ChunkBlockPos cbpos;
    chunkManager->relativePosToChunkBlockPos(chunk, x, y, z, cbpos);
    if(cbpos.isValid())
    {
        updateList.push_back(cbpos.chunk);
        if(cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z) + 2 <= lightlevel && cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_TRANSPARENT))
        {
            cbpos.chunk->setTorchlight(cbpos.x, cbpos.y, cbpos.z, lightlevel-1);
            // Emplace new node to queue. (could use push as well)
            //SDL_Log("Placing pos %d,%d,%d level %d", x,y,z, lightlevel);
            lightQueue.emplace(chunk, x, y, z);
        }
    }
    else
    {
        SDL_Log("Can't propagate light into invalid position %d,%d,%d", x, y, z);
    }
}

void LightMapper::propagateTorchLightRemoval(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel)
{
    ChunkBlockPos cbpos;
    chunkManager->relativePosToChunkBlockPos(chunk, x, y, z, cbpos);
    if(cbpos.isValid())
    {
        updateList.push_back(cbpos.chunk);
        int neighbourLevel = cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z);
        if(neighbourLevel != 0 && neighbourLevel < lightlevel && cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_TRANSPARENT))
        {
            cbpos.chunk->setTorchlight(cbpos.x, cbpos.y, cbpos.z, 0);
            lightRemovalQueue.emplace(chunk, x, y, z, neighbourLevel);
        }
        else
        {
            lightQueue.emplace(chunk, x, y, z);
        }
    }
    else
    {
        SDL_Log("Can't propagate light removal into invalid position %d,%d,%d", x, y, z);
    }
}

void LightMapper::processLightQueue()
{
    ChunkBlockPos cbpos;
    while(!lightQueue.empty())
    {
        // Get a reference to the front node.
        auto &node = lightQueue.front();
        Chunk* chunk = node.chunk;
        lightQueue.pop();

        //SDL_Log("Reading lightnode pos %d,%d,%d", node.x, node.y, node.z);

        // Grab the light level of the current node
        chunkManager->relativePosToChunkBlockPos(chunk, node.x, node.y, node.z, cbpos);
        if(!cbpos.isValid())
        {
            SDL_Log("Current lightnode %d,%d,%d position invalid", node.x, node.y, node.z);
            continue;
        }
        i32 lightlevel = cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z);
        //SDL_Log("Current lightnode %d,%d,%d light level %d", node.x, node.y, node.z, lightlevel);

        // propagate light to all 6 surrounding blocks
        // sides
        propagateTorchLight(chunk, node.x - 1, node.y, node.z, lightlevel);
        propagateTorchLight(chunk, node.x + 1, node.y, node.z, lightlevel);

        // top bottom
        propagateTorchLight(chunk, node.x, node.y - 1, node.z, lightlevel);
        propagateTorchLight(chunk, node.x, node.y + 1, node.z, lightlevel);

        // front and back
        propagateTorchLight(chunk, node.x, node.y, node.z - 1, lightlevel);
        propagateTorchLight(chunk, node.x, node.y, node.z + 1, lightlevel);
    }
}

void LightMapper::processLightRemovalQueue()
{
    ChunkBlockPos cbpos;
    while(!lightRemovalQueue.empty())
    {
        // Get a reference to the front node.
        auto &node = lightRemovalQueue.front();
        Chunk* chunk = node.chunk;
        lightRemovalQueue.pop();

        //SDL_Log("Reading lightnode pos %d,%d,%d", node.x, node.y, node.z);

        chunkManager->relativePosToChunkBlockPos(chunk, node.x, node.y, node.z, cbpos);
        if(!cbpos.isValid())
        {
            SDL_Log("Current lightremovalnode %d,%d,%d position invalid", node.x, node.y, node.z);
            continue;
        }

        // propagate light to all 6 surrounding blocks
        i32 lightlevel = node.val;
        //SDL_Log("Current lightremovalnode %d,%d,%d level %d", node.x, node.y, node.z, lightlevel);
        // sides
        propagateTorchLightRemoval(chunk, node.x - 1, node.y, node.z, lightlevel);
        propagateTorchLightRemoval(chunk, node.x + 1, node.y, node.z, lightlevel);

        // top bottom
        propagateTorchLightRemoval(chunk, node.x, node.y - 1, node.z, lightlevel);
        propagateTorchLightRemoval(chunk, node.x, node.y + 1, node.z, lightlevel);

        // front and back
        propagateTorchLightRemoval(chunk, node.x, node.y, node.z - 1, lightlevel);
        propagateTorchLightRemoval(chunk, node.x, node.y, node.z + 1, lightlevel);
    }
}

// does a BFS search (brett first search ;)
void LightMapper::placeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z, u8 level) {
    SDL_Log("Placing torchlight at %d,%d,%d light level %d", x, y ,z, level);
    origin_chunk->setTorchlight(x,y,z,level);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].setFlag(BLOCK_FLAG_ACTIVE, true);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].type = BLOCK_GOLD;

    lightQueue.emplace(origin_chunk, x, y, z);
    updateList.push_back(origin_chunk);
    processLightQueue();
    std::sort(updateList.begin(), updateList.end());
    updateList.erase( std::unique(updateList.begin(), updateList.end()), updateList.end());
    SDL_Log("updateList.size = %d", updateList.size());
}

void LightMapper::removeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z) {
    auto val = origin_chunk->getTorchlight(x,y,z);
    SDL_Log("Removing torchlight at %d,%d,%d light level %d", x, y ,z, val);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].setFlag(BLOCK_FLAG_ACTIVE, false);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
    origin_chunk->blocks[POS_TO_INDEX(y, z, x)].setFlag(BLOCK_FLAG_SHOULD_MESH, false);
    origin_chunk->setTorchlight(x,y,z,0);
    updateList.push_back(origin_chunk);
    lightRemovalQueue.emplace(origin_chunk, x, y, z, val);
    processLightRemovalQueue();
    processLightQueue();
    std::sort(updateList.begin(), updateList.end());
    updateList.erase( std::unique(updateList.begin(), updateList.end()), updateList.end());
    SDL_Log("updateList.size = %d", updateList.size());
}


