//
// Created by bison on 17-12-2017.
//

#include "Chunk.h"

Chunk::Chunk() {
    // Create the blocks
    blocks = new Block**[CHUNK_SIZE];
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        blocks[i] = new Block*[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            blocks[i][j] = new Block[CHUNK_SIZE];
        }
    }
}

Chunk::~Chunk() {
    // Delete the blocks
    for (int i = 0; i < CHUNK_SIZE; ++i)
    {
        for (int j = 0; j < CHUNK_SIZE; ++j)
        {
            delete [] blocks[i][j];
        }

        delete [] blocks[i];
    }
    delete [] blocks;
}

void Chunk::update(float dt) {

}

void Chunk::render() {

}
