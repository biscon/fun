//
// Created by bison on 17-12-2017.
//

#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H

// block types
#define BLOCK_STONE 0
#define BLOCK_GRASS 1
#define BLOCK_WATER 2
#define BLOCK_EMERALD 3
#define BLOCK_GOLD 4
#define BLOCK_SILVER 5
#define BLOCK_BRONZE 6
#define BLOCK_DEBUG 7

#define BLOCK_PLANKS 8
#define BLOCK_GLASS 9
#define BLOCK_BRICK 10
#define BLOCK_SAND 11
#define BLOCK_RED_SAND 12

// block flags
#define BLOCK_FLAG_ACTIVE 0x01
#define BLOCK_FLAG_SHOULD_MESH 0x02
#define BLOCK_FLAG_TRANSPARENT 0x04
#define BLOCK_FLAG_UNUSED2 0x08
#define BLOCK_FLAG_UNUSED3 0x10
#define BLOCK_FLAG_UNUSED4 0x20
#define BLOCK_FLAG_UNUSED5 0x40
#define BLOCK_FLAG_UNUSED6 0x80


#include "defs.h"

struct BlockFaces {
    bool back = true, front = true, left = true, right = true, bottom = true, top = true;
    inline void disableAll()
    {
        back = false;
        front = false;
        left = false;
        right = false;
        bottom = false;
        top = false;
    }
    inline void enableAll()
    {
        back = true;
        front = true;
        left = true;
        right = true;
        bottom = true;
        top = true;
    }
    inline bool anyFacesEnabled()
    {
        return back || front || left || right || bottom || top;
    }
};

struct Block {
    u8 type = 0;
    u8 flags = 0;

    inline bool isFlagSet(u8 mask) {
        return (flags & mask) != 0;
    }

    inline void setFlag(u8 mask, bool val) {
        if(val)
            flags |= mask;
        else
            flags &= ~mask;
    }
};


#endif //GAME_BLOCK_H
