//
// Created by bison on 29-12-2017.
//

#ifndef GAME_CHUNKPOS_H
#define GAME_CHUNKPOS_H


#include "defs.h"

struct ChunkPos
{
    i32 x;
    i32 z;

    bool operator<(const ChunkPos &rhs) const {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return z < rhs.z;
    }

    inline void set(i32 nx, i32 nz)
    {
        x = nx;
        z = nz;
    }
};



#endif //GAME_CHUNKPOS_H
