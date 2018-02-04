//
// Created by bison on 14-01-2018.
//

#ifndef GAME_CHUNKBLOCKPOS_H
#define GAME_CHUNKBLOCKPOS_H


//#include "Chunk.h"
#include <defs.h>

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

namespace std {
    template <>
    struct hash<ChunkBlockPos>
    {
        inline std::size_t operator()(const ChunkBlockPos& k) const
        {
            u64 h = 0xcbf29ce484222325;
            u8 *x = (u8*) &k.x;
            u8 *y = (u8*) &k.y;
            u8 *z = (u8*) &k.z;
            h = (h ^ (u64)x[0]) * 0x100000001b3;
            h = (h ^ (u64)x[1]) * 0x100000001b3;
            h = (h ^ (u64)y[0]) * 0x100000001b3;
            h = (h ^ (u64)y[1]) * 0x100000001b3;
            h = (h ^ (u64)z[0]) * 0x100000001b3;
            h = (h ^ (u64)z[1]) * 0x100000001b3;
            return h;
            /*
            auto A = (u64)(k.x >= 0 ? 2 * (i64)k.x : -2 * (i64)k.x - 1);
            auto B = (u64)(k.z >= 0 ? 2 * (i64)k.z : -2 * (i64)k.z - 1);
            auto C = (i64)((A >= B ? A * A + A + B : A + B * B) / 2);
            return k.x < 0 && k.z < 0 || k.x >= 0 && k.z >= 0 ? C : -C - 1;
             */
        }
    };
}

#endif //GAME_CHUNKBLOCKPOS_H
