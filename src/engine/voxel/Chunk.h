//
// Created by bison on 17-12-2017.
//

#ifndef GAME_CHUNK_H
#define GAME_CHUNK_H


#include <memory>
#include <engine/video/Mesh2.h>
#include "Block.h"

class Chunk {
public:
    Chunk();
    virtual ~Chunk();

    void update(float dt);
    void render();

    static const int CHUNK_SIZE = 16;
private:
    Block*** blocks;
    std::unique_ptr<Mesh2> mesh;
};


#endif //GAME_CHUNK_H
