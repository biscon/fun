//
// Created by bison on 19-01-2018.
//

#ifndef GAME_LIGHTMAPPER_H
#define GAME_LIGHTMAPPER_H

#include <memory>
#include <queue>
#include "Chunk.h"

#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3
#define NORTH_WEST 4
#define NORTH_EAST 5
#define SOUTH_WEST 6
#define SOUTH_EAST 7
#define UP 8
#define DOWN 9

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
    void calculateSunlight(Chunk *chunk);

    bool clipping = false;

private:
    IChunkManager *chunkManager;
    std::vector<Chunk*> &updateList;
    void propagateLight(std::queue<LightNode> &light_queue, Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel, bool is_sunlight, bool do_not_attenuate);
    void propagateLightRemoval(Chunk *chunk, int32_t x, int32_t y, int32_t z, int32_t lightlevel);
    void processLightQueue(std::queue<LightNode> &light_queue, bool is_sunlight);
    void processLightRemovalQueue();

    std::queue<LightNode> lightQueue;
    std::queue<LightNode> sunLightQueue;
    std::queue<LightRemovalNode> lightRemovalQueue;
};


#endif //GAME_LIGHTMAPPER_H
