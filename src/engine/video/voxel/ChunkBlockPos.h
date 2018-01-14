//
// Created by bison on 14-01-2018.
//

#ifndef GAME_CHUNKBLOCKPOS_H
#define GAME_CHUNKBLOCKPOS_H


#include "Chunk.h"

class Chunk;

struct ChunkBlockPos
{
    i32 x = 0;
    i32 y = 0;
    i32 z = 0;
    Chunk *chunk;

    inline void setInvalid()
    {
        x = y = z = -1;
        chunk = nullptr;
    }

    inline bool isValid()
    {
        if(chunk != nullptr)
            return true;
        return false;
    }
};

#endif //GAME_CHUNKBLOCKPOS_H
