//
// Created by bison on 14-01-2018.
//

#ifndef GAME_ICHUNKMANAGER_H
#define GAME_ICHUNKMANAGER_H


#include "Chunk.h"
#include "ChunkBlockPos.h"

class IChunkManager {
public:
    virtual void relativePosToChunkBlockPos(Chunk *chunk, i32 x, i32 y, i32 z, ChunkBlockPos& chunkBlockPos)=0;
    virtual Chunk *findActiveChunkAt(const ChunkPos &pos)=0;
    virtual Chunk *findBuildChunkAt(const ChunkPos &pos)=0;
};


#endif //GAME_ICHUNKMANAGER_H
