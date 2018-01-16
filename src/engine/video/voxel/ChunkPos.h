//
// Created by bison on 29-12-2017.
//

#ifndef GAME_CHUNKPOS_H
#define GAME_CHUNKPOS_H


#include "defs.h"
#include "Chunk.h"

struct ChunkPos
{
    ChunkPos() {x = 0; z = 0;}

    ChunkPos(int32_t x, int32_t z) : x(x), z(z) {}

    i32 x;
    i32 z;

    bool operator<(const ChunkPos &rhs) const {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return z < rhs.z;
    }

    bool operator==(const ChunkPos &other) const
    {
        return (x == other.x && z == other.z);
    }

    inline void set(i32 nx, i32 nz)
    {
        x = nx;
        z = nz;
    }
};

namespace std {
    template <>
    struct hash<ChunkPos>
    {
        inline std::size_t operator()(const ChunkPos& k) const
        {
            u64 h = 0xcbf29ce484222325;
            u8 *x = (u8*) &k.x;
            u8 *z = (u8*) &k.z;
            h = (h ^ (u64)x[0]) * 0x100000001b3;
            h = (h ^ (u64)x[1]) * 0x100000001b3;
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

#endif //GAME_CHUNKPOS_H
