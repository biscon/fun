//
// Created by bison on 26-11-2017.
//

#ifndef GAME_TILEMAP_H
#define GAME_TILEMAP_H


#include <memory>
#include <vector>

struct TileType
{

};

struct Tile
{

};

class TileMap {
public:

private:
    std::vector<std::unique_ptr<TileType>> tileTypes;
    Tile* tiles;
};


#endif //GAME_TILEMAP_H
