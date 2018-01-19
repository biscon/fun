//
// Created by bison on 19-01-2018.
//

#ifndef GAME_LIGHTMAPPER_H
#define GAME_LIGHTMAPPER_H

#include <memory>
#include <queue>
#include "Chunk.h"

struct LightNode {
    LightNode(Chunk* chunk, i32 x, i32 y, i32 z) : chunk(chunk), x(x), y(y), z(z) {}
    Chunk* chunk;
    i32 x,y,z;
};

struct LightRemovalNode {

    LightRemovalNode(Chunk* chunk, i32 x, i32 y, i32 z, short v) : val(v), chunk(chunk), x(x), y(y), z(z) {}
    short val;
    i32 x,y,z;
    Chunk* chunk; //pointer to the chunk that owns it!
};

class LightMapper {
public:
    LightMapper(IChunkManager *chunkManager, std::vector<Chunk *> &updateList);

    void placeTorchLight(Chunk *origin_chunk, int x, int y, int z, u8 level);
    void removeTorchLight(Chunk *origin_chunk, i32 x, i32 y, i32 z);

private:
    IChunkManager *chunkManager;
    std::vector<Chunk*> &updateList;
    void propagateTorchLight(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel);
    void propagateTorchLightRemoval(Chunk *chunk, int32_t x, int32_t y, int32_t z, int32_t lightlevel);
    void processLightQueue();
    void processLightRemovalQueue();

    std::queue<LightNode> lightQueue;
    std::queue<LightNode> sunLightQueue;
    std::queue<LightRemovalNode> lightRemovalQueue;
};


#endif //GAME_LIGHTMAPPER_H
