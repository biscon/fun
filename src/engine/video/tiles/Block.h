//
// Created by bison on 17-12-2017.
//

#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H

#include <cstdint>

struct Block {
    static const int GRASS = 0;
    static const int STONE = 1;
    static const int DIRT = 2;
    uint32_t type = 0;
    bool active = true;
};


#endif //GAME_BLOCK_H
