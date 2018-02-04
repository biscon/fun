//
// Created by bison on 26-12-2017.
//

#include <algorithm>
#include <engine/util/string_util.h>
#include "BlockTypeDictionary.h"
#include "AmbientOcclusion.h"

i32 BlockType::layersUniformLocation = 0;
i32 BlockType::colorsUniformLocation = 0;


BlockTypeDictionary::BlockTypeDictionary() {
    arrayTexture = std::unique_ptr<OGLArrayTexture>(new OGLArrayTexture());
}

bool BlockTypeDictionary::load(IGame &game) {
    for(auto it : textureLayerMap)
    {
        layerBufferMap[it.second] = std::make_shared<PixelBuffer>(it.first);
    }
    return true;
}

bool BlockTypeDictionary::prepare(IGame &game) {
    arrayTexture->init(faceTextureSize, faceTextureSize, (i32) layerBufferMap.size());
    for(auto it : layerBufferMap)
    {
        arrayTexture->uploadLayer(it.second.get(), it.first);
    }
    arrayTexture->finalizeUpload();
    textureLayerMap.clear();
    layerBufferMap.clear();
    return true;
}

i32 BlockTypeDictionary::addLayerIfNotPresent(std::string filename)
{
    auto it = textureLayerMap.find(filename);
    if(it == textureLayerMap.end())
    {
        textureLayerMap[filename] = curLayer;
    }
    else
    {
        return it->second;
    }
    SDL_Log("Added textured material %s as layer %d", filename.c_str(), curLayer);
    return curLayer++;
}

void BlockTypeDictionary::createBlockType(std::string name, std::string diffuseMapFilename) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->texture = arrayTexture.get();
    // use the same texture for all faces
    for(i32 i = 0; i < 6; i++)
    {
        bt->material.layers[i] = addLayerIfNotPresent(diffuseMapFilename);
        bt->material.colors[i] = glm::vec4(0,0,0,0);
    }
    /*
    bt->material.layers[BACK_FACE] = -1;
    bt->material.colors[BACK_FACE] = glm::vec4(1,0,0,1);

    bt->material.layers[FRONT_FACE] = -1;
    bt->material.colors[FRONT_FACE] = glm::vec4(0,1,0,1);

    bt->material.layers[LEFT_FACE] = -1;
    bt->material.colors[LEFT_FACE] = glm::vec4(0,0,1,1);

    bt->material.layers[RIGHT_FACE] = -1;
    bt->material.colors[RIGHT_FACE] = glm::vec4(1,0,1,1);

    bt->material.layers[BOTTOM_FACE] = -1;
    bt->material.colors[BOTTOM_FACE] = glm::vec4(0,0,0,1);

    bt->material.layers[TOP_FACE] = -1;
    bt->material.colors[TOP_FACE] = glm::vec4(1,1,1,1);
     */
}

BlockType &BlockTypeDictionary::getBlockTypeAt(int index) {
    return *blockTypes[index];
}

void BlockTypeDictionary::createBlockType(std::string name, glm::vec4 diffuse) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->texture = arrayTexture.get();
    // use the same color for all faces
    for(i32 i = 0; i < 6; i++)
    {
        bt->material.colors[i] = diffuse;
        bt->material.layers[i] = -1;
    }
}

BlockType *BlockTypeDictionary::getBlockTypeByName(std::string name) {
    for(const auto& bt : blockTypes)
    {
        if(bt->name == name)
            return bt.get();
    }
    return nullptr;
}

void BlockTypeDictionary::setFaceTexture(BlockType *bt, i32 face, std::string diffuseMapFilename) {
    bt->material.layers[face] = addLayerIfNotPresent(diffuseMapFilename);
}

void BlockTypeDictionary::setFaceColor(BlockType *bt, i32 face, glm::vec4 diffuse) {
    bt->material.layers[face] = -1;
    bt->material.colors[face] = diffuse;
}
