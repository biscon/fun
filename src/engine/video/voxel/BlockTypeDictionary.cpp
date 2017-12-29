//
// Created by bison on 26-12-2017.
//

#include "BlockTypeDictionary.h"

BlockTypeDictionary::BlockTypeDictionary() {
    diffuseAtlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(1024, 1024, false));
    specularAtlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(1024, 1024, false));

}

bool BlockTypeDictionary::load(IGame &game) {
    diffuseAtlas->build();
    specularAtlas->build();
    return true;
}

bool BlockTypeDictionary::prepare(IGame &game) {
    diffuseAtlas->upload();
    specularAtlas->upload();
    return true;
}

void BlockTypeDictionary::createTileType(std::string name, std::string diffuseMapFilename, std::string specularMapFilename, float shininess) {
    tileTypes.push_back(std::unique_ptr<TileType>(new TileType()));
    auto& tt = tileTypes.back();
    tt->name = name;
    tt->diffuseMapFilename = diffuseMapFilename;
    tt->specularMapFilename = specularMapFilename;
    tt->shininess = shininess;
    tt->diffuseMapHandle = diffuseAtlas->addBuffer(std::make_shared<PixelBuffer>(diffuseMapFilename));
    tt->specularMapHandle = specularAtlas->addBuffer(std::make_shared<PixelBuffer>(specularMapFilename));
}

TileType &BlockTypeDictionary::getTileTypeAt(int index) {
    return *tileTypes[index];
}
