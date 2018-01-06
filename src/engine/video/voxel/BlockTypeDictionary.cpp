//
// Created by bison on 26-12-2017.
//

#include "BlockTypeDictionary.h"

BlockTypeDictionary::BlockTypeDictionary() {

}

bool BlockTypeDictionary::load(IGame &game) {
    for(auto const& bt : blockTypes)
    {
        if(!bt->material.diffuseMap.empty())
            bt->diffusePb = std::make_shared<PixelBuffer>(bt->material.diffuseMap);
        if(!bt->material.specularMap.empty())
            bt->specularPb = std::make_shared<PixelBuffer>(bt->material.specularMap);
        //SDL_Log("Loading textures for block = %s, diffuse = %s, spec = %s", bt->name.c_str(), bt->material.diffuseMap.c_str(), bt->material.specularMap.c_str());
    }
    return true;
}

bool BlockTypeDictionary::prepare(IGame &game) {
    for(auto const& bt : blockTypes)
    {
        if(bt->diffusePb != nullptr)
            bt->material.diffuseTexture = std::make_shared<OGLTexture>(bt->diffusePb.get(), "texture_diffuse", true);
        if(bt->specularPb != nullptr)
            bt->material.specularTexture = std::make_shared<OGLTexture>(bt->specularPb.get(), "texture_specular", true);
        //SDL_Log("Uploading Block = %s, diffusemap = %s (id %d), specmap = %s (id %d)", bt->name.c_str(), bt->material.diffuseMap.c_str(), bt->material.diffuseTexture->tex, bt->material.specularMap.c_str(), bt->material.specularTexture->tex);
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

void BlockTypeDictionary::createBlockType(std::string name, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
                                          float shininess) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->material.ambient = ambient;
    bt->material.diffuse = diffuse;
    bt->material.specular = specular;
    bt->material.shininess = shininess;
}
