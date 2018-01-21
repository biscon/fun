//
// Created by bison on 19-01-2018.
//

#include <algorithm>
#include "LightMapper.h"

LightMapper::LightMapper(IChunkManager *chunkManager, std::vector<Chunk *> &updateList) : chunkManager(chunkManager),
                                                                                          updateList(updateList) {}

void LightMapper::propagateLight(std::queue<LightNode> &light_queue, Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel,
                                 bool is_sunlight, bool do_not_attenuate)
{

    if(is_sunlight && clipping)
    {
        if(x < 0 || x > CHUNK_SIZE-1 || y < 0 || y > CHUNK_HEIGHT-1 || z < 0 || z > CHUNK_SIZE-1)
            return;
    }

    ChunkBlockPos cbpos;
    chunkManager->relativePosToChunkBlockPos(chunk, x, y, z, cbpos);
    if(cbpos.isValid())
    {
        if(!is_sunlight) {
            updateList.push_back(cbpos.chunk);
            if (cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z) + 2 <= lightlevel &&
                cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_TRANSPARENT))
            {

                cbpos.chunk->setTorchlight(cbpos.x, cbpos.y, cbpos.z, lightlevel - 1);
                light_queue.emplace(chunk, x, y, z);
            }
        }
        else
        {
            if (cbpos.chunk->getSunlight(cbpos.x, cbpos.y, cbpos.z) + 2 <= lightlevel &&
                cbpos.chunk->blocks[POS_TO_INDEX(cbpos.y, cbpos.z, cbpos.x)].isFlagSet(BLOCK_FLAG_TRANSPARENT))
            {
                if(!do_not_attenuate)
                    cbpos.chunk->setSunlight(cbpos.x, cbpos.y, cbpos.z, lightlevel - 1);
                else
                    cbpos.chunk->setSunlight(cbpos.x, cbpos.y, cbpos.z, lightlevel);
                light_queue.emplace(chunk, x, y, z);
            }
        }
    }
    else
    {
        if(y > 0 && y < CHUNK_HEIGHT && chunk->chunkPosition.x == 0 && chunk->chunkPosition.z == 0)
            SDL_Log("Can't propagate light into invalid position %d,%d,%d", x, y, z);
    }
}

void LightMapper::propagateLightRemoval(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel)
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
        //SDL_Log("Can't propagate light removal into invalid position %d,%d,%d", x, y, z);
    }
}

void LightMapper::processLightQueue(std::queue<LightNode> &light_queue, bool is_sunlight)
{
    ChunkBlockPos cbpos;
    while(!light_queue.empty())
    {
        // Get a reference to the front node.
        auto &node = light_queue.front();
        Chunk* chunk = node.chunk;
        light_queue.pop();

        //SDL_Log("Reading lightnode pos %d,%d,%d", node.x, node.y, node.z);

        // Grab the light level of the current node
        chunkManager->relativePosToChunkBlockPos(chunk, node.x, node.y, node.z, cbpos);
        if(!cbpos.isValid())
        {
            SDL_Log("Current lightnode %d,%d,%d position invalid", node.x, node.y, node.z);
            continue;
        }
        i32 lightlevel = 0;
        if(!is_sunlight)
            lightlevel = cbpos.chunk->getTorchlight(cbpos.x, cbpos.y, cbpos.z);
        else
            lightlevel = cbpos.chunk->getSunlight(cbpos.x, cbpos.y, cbpos.z);
        //SDL_Log("Current lightnode %d,%d,%d light level %d", node.x, node.y, node.z, lightlevel);

        // propagate light to all 6 surrounding blocks
        // sides
        propagateLight(light_queue, chunk, node.x - 1, node.y, node.z, lightlevel, is_sunlight, false);
        propagateLight(light_queue, chunk, node.x + 1, node.y, node.z, lightlevel, is_sunlight, false);

        // top bottom
        if(is_sunlight)
            propagateLight(light_queue, chunk, node.x, node.y - 1, node.z, lightlevel, is_sunlight, true);
        else
            propagateLight(light_queue, chunk, node.x, node.y - 1, node.z, lightlevel, is_sunlight, false);
        if(!is_sunlight)
            propagateLight(light_queue, chunk, node.x, node.y + 1, node.z, lightlevel, is_sunlight, false);

        // front and back
        propagateLight(light_queue, chunk, node.x, node.y, node.z - 1, lightlevel, is_sunlight, false);
        propagateLight(light_queue, chunk, node.x, node.y, node.z + 1, lightlevel, is_sunlight, false);
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
        propagateLightRemoval(chunk, node.x - 1, node.y, node.z, lightlevel);
        propagateLightRemoval(chunk, node.x + 1, node.y, node.z, lightlevel);

        // top bottom
        propagateLightRemoval(chunk, node.x, node.y - 1, node.z, lightlevel);
        propagateLightRemoval(chunk, node.x, node.y + 1, node.z, lightlevel);

        // front and back
        propagateLightRemoval(chunk, node.x, node.y, node.z - 1, lightlevel);
        propagateLightRemoval(chunk, node.x, node.y, node.z + 1, lightlevel);
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
    processLightQueue(lightQueue, false);
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
    processLightQueue(lightQueue, false);
    std::sort(updateList.begin(), updateList.end());
    updateList.erase( std::unique(updateList.begin(), updateList.end()), updateList.end());
    SDL_Log("updateList.size = %d", updateList.size());
}

/*
void LightMapper::calculateSunlight(Chunk *chunk) {
    // iterate the top layer of the sun light map
    i32 y = CHUNK_HEIGHT-2;
    for(i32 z = 0; z < CHUNK_SIZE; z++)
    {
        for(i32 x = 0; x < CHUNK_SIZE; x++)
        {
            Block &block = chunk->blocks[POS_TO_INDEX(y, z, x)];
            if(block.isFlagSet(BLOCK_FLAG_TRANSPARENT))
            {
                // set to max intensity and add node to light queue
                //SDL_Log("Setting sunlight to max %d,%d,%d", x, y, z);
                chunk->setSunlight(x, y, z, 15);
                sunLightQueue.emplace(chunk, x, y, z);
            }
        }
    }
    processLightQueue(sunLightQueue, true);
}
*/

static inline u8 getSunLightAndIncreaseCount(Chunk *chunk, i32 x, i32 y, i32 z, i32 &count) {
    if(chunk->isBlockTransparentAt(x, y, z))
    {
        count++;
        return chunk->getSunLightAt(x, y, z);
    }
    return 0;
}


/*
 * Start with the topmost layer (slice of constant height). Fill it with sunlight.
 * Then go to the layer below, and every voxel there gets its illumination from the closest voxels in the previous layer (above it).
 * Put zero light in solid voxels. You have a couple of ways to decide the "kernel", the weights of the contributions from the voxels above,
 * Minecraft uses the maximum value found, but decreases it by 1 if the propagation is not straight down.
 * This is the lateral attenuation, so unblocked vertical columns of voxels will get the full sunlight propagation and light bends around corners.
 */

/*
void LightMapper::calculateSunlight(Chunk *chunk) {
    bool is_origin = false;
    if(chunk->chunkPosition.x == 0 && chunk->chunkPosition.z == 0)
        is_origin = true;

    // fill the top layer of the sun light map
    i32 y = CHUNK_HEIGHT-1;
    for(i32 z = 0; z < CHUNK_SIZE; z++)
    {
        for(i32 x = 0; x < CHUNK_SIZE; x++)
        {
            Block &block = chunk->blocks[POS_TO_INDEX(y, z, x)];
            if(block.isFlagSet(BLOCK_FLAG_TRANSPARENT))
            {
                chunk->setSunlight(x, y, z, 15);
            }
        }
    }
    // propagate with a kernel, iterate from next highest layer to bottom
    i32 high_val = 0;
    i32 dir = 0;
    i32 kernel[9];

    for(i32 y = CHUNK_HEIGHT-1; y-- > 0;) {
        for (i32 z = 0; z < CHUNK_SIZE; z++) {
            for (i32 x = 0; x < CHUNK_SIZE; x++) {
                //SDL_Log("Light processing %d,%d,%d", x, y, z);
                Block &block = chunk->blocks[POS_TO_INDEX(y, z, x)];
                if (block.isFlagSet(BLOCK_FLAG_TRANSPARENT)) {
                    memset(kernel, 0 , sizeof(kernel));

                    // up
                    if(chunk->isBlockTransparentAt(x, y+1, z)) {
                        kernel[UP] = chunk->getSunLightAt(x, y+1, z);
                        chunk->setSunlight(x, y, z, kernel[UP]);
                        continue;
                    }

                    // north
                    if(chunk->isBlockTransparentAt(x, y, z-1))
                        kernel[NORTH] = chunk->getSunLightAt(x, y, z-1);

                    // south
                    if(chunk->isBlockTransparentAt(x, y, z+1))
                        kernel[SOUTH] = chunk->getSunLightAt(x, y, z+1);

                    // west
                    if(chunk->isBlockTransparentAt(x-1, y, z))
                        kernel[WEST] = chunk->getSunLightAt(x-1, y, z);

                    // east
                    if(chunk->isBlockTransparentAt(x+1, y, z))
                        kernel[EAST] = chunk->getSunLightAt(x+1, y, z);

                    // above
                    if(chunk->isBlockTransparentAt(x, y+1, z-1))
                        kernel[NORTH_WEST] = chunk->getSunLightAt(x, y+1, z-1);

                    // south
                    if(chunk->isBlockTransparentAt(x, y+1, z+1))
                        kernel[NORTH_EAST] = chunk->getSunLightAt(x, y+1, z+1);

                    // west
                    if(chunk->isBlockTransparentAt(x-1, y+1, z))
                        kernel[SOUTH_WEST] = chunk->getSunLightAt(x-1, y+1, z);

                    // east
                    if(chunk->isBlockTransparentAt(x+1, y+1, z))
                        kernel[SOUTH_EAST] = chunk->getSunLightAt(x+1, y+1, z);




                    high_val = -1;
                    dir = -1;
                    for(i32 i = 0; i < 8; i++)
                    {
                        if(kernel[i] >= high_val)
                        {
                            high_val = kernel[i];
                            dir = i;
                        }
                    }

                    if(high_val < 0)
                        high_val = 0;

                    if(dir == UP) {
                        //SDL_Log("dang");
                        chunk->setSunlight(x, y, z, high_val);
                    }
                    else {
                        if(high_val > 0)
                            chunk->setSunlight(x, y, z, high_val - 1);
                        else
                            chunk->setSunlight(x, y, z, 0);
                    }

                    //if(is_origin && dir != UP)
                        //SDL_Log("Light processing %d,%d,%d dir %d", x, y, z, dir);
                }
                //else
                    //chunk->setSunlight(x,y,z,0);
            }
        }
    }
}
*/

void LightMapper::calculateSunlight(Chunk *chunk) {
    bool is_origin = false;
    if(chunk->chunkPosition.x == 0 && chunk->chunkPosition.z == 0)
        is_origin = true;

    u8 mask[CHUNK_SIZE][CHUNK_SIZE];

    for(i32 z = 0; z < CHUNK_SIZE; z++)
    {
        for(i32 x = 0; x < CHUNK_SIZE; x++)
        {
            mask[x][z] = 15;
        }
    }

    for(i32 y = CHUNK_HEIGHT-1; y-- > 0;) {
        for (i32 z = 0; z < CHUNK_SIZE; z++) {
            for (i32 x = 0; x < CHUNK_SIZE; x++) {
                //SDL_Log("Iterating %d,%d,%d", x, y, z);
                Block &block = chunk->blocks[POS_TO_INDEX(y, z, x)];
                //if(y == 105 && is_origin)
                //    SDL_Log("y = 107 mask %d,%d = %d", x, z, mask[x][z]);
                if(block.isFlagSet(BLOCK_FLAG_TRANSPARENT))
                {
                    chunk->setSunlight(x, y, z, mask[x][z]);
                    if(mask[x][z] == 0)
                    {
                        if(x == 0 || x == CHUNK_SIZE-1 || z == 0 || z == CHUNK_SIZE-1)
                        {
                            //SDL_Log("Setting edge block %d,%d,%d to 14", x, y, z);
                            i32 x_level = 0;
                            i32 z_level = 0;
                            if(x == 0)
                                x_level = chunk->getSunLightAt(x - 1, y, z);
                            if(x == CHUNK_SIZE-1)
                                x_level = chunk->getSunLightAt(x + 1, y, z);

                            if(z == 0)
                                z_level = chunk->getSunLightAt(x, y, z - 1);
                            if(z == CHUNK_SIZE-1)
                                z_level = chunk->getSunLightAt(x, y, z + 1);

                            if(x_level > z_level)
                                chunk->setSunlight(x, y, z, x_level);
                            else
                                chunk->setSunlight(x, y, z, z_level);

                            sunLightQueue.emplace(chunk, x, y, z);
                        }
                    }
                }
                else
                    mask[x][z] = 0;
            }
        }
    }


    bool is_sunlight = true;
    ChunkBlockPos cbpos;
    while(!sunLightQueue.empty())
    {
        // Get a reference to the front node.
        auto &node = sunLightQueue.front();
        sunLightQueue.pop();

        //SDL_Log("Reading lightnode pos %d,%d,%d", node.x, node.y, node.z);

        // Grab the light level of the current node
        chunkManager->relativePosToChunkBlockPos(node.chunk, node.x, node.y, node.z, cbpos);
        if(!cbpos.isValid())
        {
            SDL_Log("Current lightnode %d,%d,%d position invalid", node.x, node.y, node.z);
            continue;
        }
        i32 lightlevel = 0;

        lightlevel = cbpos.chunk->getSunlight(cbpos.x, cbpos.y, cbpos.z);
        // propagate light to all 6 surrounding blocks
        // sides
        propagateLight(sunLightQueue, chunk, node.x - 1, node.y, node.z, lightlevel, is_sunlight, false);
        propagateLight(sunLightQueue, chunk, node.x + 1, node.y, node.z, lightlevel, is_sunlight, false);

        // top bottom
        propagateLight(sunLightQueue, chunk, node.x, node.y - 1, node.z, lightlevel, is_sunlight, false);
        propagateLight(sunLightQueue, chunk, node.x, node.y + 1, node.z, lightlevel, is_sunlight, false);

        // front and back
        propagateLight(sunLightQueue, chunk, node.x, node.y, node.z - 1, lightlevel, is_sunlight, false);
        propagateLight(sunLightQueue, chunk, node.x, node.y, node.z + 1, lightlevel, is_sunlight, false);
    }

    /*
    if(is_origin)
        exit(-1);
        */
}

