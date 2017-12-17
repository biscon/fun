//
// Created by bison on 17-12-2017.
//

#ifndef GAME_BLOCK_H
#define GAME_BLOCK_H

enum class BlockType
{
    DEFAULT = 0,
    GRASS,
    DIRT,
    WATER,
    STONE,
    WOOD,
    SAND,
    NUM_TYPES
};

class Block {
public:
    BlockType type;
    bool active;

private:

};


#endif //GAME_BLOCK_H
