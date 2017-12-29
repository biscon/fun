//
// Created by bison on 26-12-2017.
//

#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/Mesh3.h>
#include <engine/video/TextureAtlas.h>
#include "BlockTypeDictionary.h"
#include "Block.h"

class Chunk {
public:
    static const int32_t CHUNK_SIZE = 16;
    Chunk(BlockTypeDictionary &tileTypeDict);

    virtual ~Chunk();

    void rebuild();
    void draw(const Shader& shader);
    glm::vec3 position;
    Block*** blocks;

private:
    BlockTypeDictionary& tileTypeDict;
    std::unique_ptr<Mesh3> mesh;

    bool isBlockActiveAt(int32_t x, int32_t y, int32_t z);

    void randomizeHeights();
};


#endif //GAME_TILECHUNK_H
