//
// Created by bison on 26-12-2017.
//

#ifndef GAME_TILETYPEDICTIONARY_H
#define GAME_TILETYPEDICTIONARY_H


#include <memory>
#include <string>
#include <vector>
#include <engine/video/TextureAtlas.h>
#include <engine/asset/ILoadTask.h>

struct TileType
{
    std::string name;
    std::string diffuseMapFilename;
    std::string specularMapFilename;
    int32_t diffuseMapHandle;
    int32_t specularMapHandle;
    float shininess;
};

class BlockTypeDictionary : public ILoadTask {
public:
    BlockTypeDictionary();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void createTileType(std::string name, std::string diffuseMapFilename, std::string specularMapFilename, float shininess);
    TileType& getTileTypeAt(int index);
    std::unique_ptr<TextureAtlas> diffuseAtlas;
    std::unique_ptr<TextureAtlas> specularAtlas;

private:
    std::vector<std::unique_ptr<TileType>> tileTypes;
};


#endif //GAME_TILETYPEDICTIONARY_H
