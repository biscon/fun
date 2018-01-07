//
// Created by bison on 17-12-2017.
//

#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H

#define BLOCK_STONE 0
#define BLOCK_GRASS 1
#define BLOCK_WATER 2
#define BLOCK_EMERALD 3
#define BLOCK_GOLD 4
#define BLOCK_SILVER 5
#define BLOCK_BRONZE 6
#include "defs.h"

struct BlockFaces {
    bool back = true, front = true, left = true, right = true, bottom = true, top = true;
};

struct Block {
    uint8_t type = 0;
    // TODO this takes up a byte, waste of memory, instead define 8 bit field to hold binary options (on/off)
    bool active = false;
};


#endif //GAME_BLOCK_H
