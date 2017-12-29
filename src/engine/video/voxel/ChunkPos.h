//
// Created by bison on 29-12-2017.
//

#ifndef GAME_CHUNKPOS_H
#define GAME_CHUNKPOS_H


#include <cstdint>

struct ChunkPos
{
    int32_t x;
    int32_t z;
    /*
    bool operator<(const ChunkPos r)
    {
        if ( l.x < r.x )  return true;
        if ( l.x > r.x )  return false;

        // Otherwise a are equal
        if ( l.z < r.z )  return true;
        if ( l.z > r.z )  return false;
        // Otherwise both are equal
        return false;
    }
    */

    bool operator<(const ChunkPos &rhs) const {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return z < rhs.z;
    }

    /*
    bool operator>(const ChunkPos &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const ChunkPos &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const ChunkPos &rhs) const {
        return !(*this < rhs);
    }
     */
};



#endif //GAME_CHUNKPOS_H
