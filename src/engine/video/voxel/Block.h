//
// Created by bison on 17-12-2017.
//

#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H

#include <cstdint>

struct Block {
    static const int STONE = 0;
    static const int GRASS = 1;
    static const int WATER = 2;
    uint32_t type = 0;
    bool active = false;
};


#endif //GAME_BLOCK_H
