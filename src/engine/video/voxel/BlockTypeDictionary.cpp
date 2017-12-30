//
// Created by bison on 26-12-2017.
//

#include "BlockTypeDictionary.h"

BlockTypeDictionary::BlockTypeDictionary() {

}

bool BlockTypeDictionary::load(IGame &game) {
    for(auto const& bt : blockTypes)
    {
        bt->diffusePb = std::make_shared<PixelBuffer>(bt->material.diffuseMap);
        bt->specularPb = std::make_shared<PixelBuffer>(bt->material.specularMap);
    }
    return true;
}

bool BlockTypeDictionary::prepare(IGame &game) {
    for(auto const& bt : blockTypes)
    {
        bt->material.diffuseTexture = std::make_shared<OGLTexture>(bt->diffusePb.get(), "texture_diffuse", true);
        bt->material.specularTexture = std::make_shared<OGLTexture>(bt->specularPb.get(), "texture_diffuse", true);
    }
    return true;
}

void BlockTypeDictionary::createBlockType(std::string name, std::string diffuseMapFilename,
                                          std::string specularMapFilename, float shininess) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->material.shininess = shininess;
    bt->material.diffuseMap = diffuseMapFilename;
    bt->material.specularMap = specularMapFilename;
}

BlockType &BlockTypeDictionary::getBlockTypeAt(int index) {
    return *blockTypes[index];
}
