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
#include <engine/video/Material.h>

struct BlockType
{
    std::string name;
    Material material;
    std::shared_ptr<PixelBuffer> diffusePb;
    std::shared_ptr<PixelBuffer> specularPb;
};

class BlockTypeDictionary : public ILoadTask {
public:
    BlockTypeDictionary();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void createBlockType(std::string name, std::string diffuseMapFilename, std::string specularMapFilename, float shininess);
    BlockType& getBlockTypeAt(int index);

private:
    std::vector<std::unique_ptr<BlockType>> blockTypes;
};


#endif //GAME_TILETYPEDICTIONARY_H
