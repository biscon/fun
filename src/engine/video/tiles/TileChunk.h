//
// Created by bison on 26-12-2017.
//

#ifndef GAME_TILECHUNK_H
#define GAME_TILECHUNK_H

#include <memory>
#include <engine/video/Mesh3.h>
#include <engine/video/TextureAtlas.h>
#include "TileTypeDictionary.h"
#include "Block.h"

class TileChunk {
public:
    static const int32_t CHUNK_SIZE = 16;
    TileChunk(TileTypeDictionary &tileTypeDict);

    virtual ~TileChunk();

    void rebuild();
    void draw(const Shader& shader);
    glm::vec3 position;
    Block*** blocks;

private:
    TileTypeDictionary& tileTypeDict;
    std::unique_ptr<Mesh3> mesh;

    bool isBlockActiveAt(int32_t x, int32_t y, int32_t z);
};


#endif //GAME_TILECHUNK_H
