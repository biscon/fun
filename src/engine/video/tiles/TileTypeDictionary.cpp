//
// Created by bison on 26-12-2017.
//

#include "TileTypeDictionary.h"

TileTypeDictionary::TileTypeDictionary() {
    diffuseAtlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(1024, 1024, false));
    specularAtlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(1024, 1024, false));

}

bool TileTypeDictionary::load(IGame &game) {
    diffuseAtlas->build();
    specularAtlas->build();
    return true;
}

bool TileTypeDictionary::prepare(IGame &game) {
    diffuseAtlas->upload();
    specularAtlas->upload();
    return true;
}

void TileTypeDictionary::createTileType(std::string name, std::string diffuseMapFilename, std::string specularMapFilename, float shininess) {
    tileTypes.push_back(std::unique_ptr<TileType>(new TileType()));
    auto& tt = tileTypes.back();
    tt->name = name;
    tt->diffuseMapFilename = diffuseMapFilename;
    tt->specularMapFilename = specularMapFilename;
    tt->shininess = shininess;
    tt->diffuseMapHandle = diffuseAtlas->addBuffer(std::make_shared<PixelBuffer>(diffuseMapFilename));
    tt->specularMapHandle = specularAtlas->addBuffer(std::make_shared<PixelBuffer>(specularMapFilename));
}

TileType &TileTypeDictionary::getTileTypeAt(int index) {
    return *tileTypes[index];
}
