//
// Created by bison on 26-12-2017.
//

#ifndef GAME_TILETYPEDICTIONARY_H
#define GAME_TILETYPEDICTIONARY_H


#include <memory>
#include <string>
#include <vector>
#include <engine/video/texture/TextureAtlas.h>
#include <engine/asset/ILoadTask.h>
#include <engine/video/shader/FaceMaterial.h>

struct BlockType
{
    std::string name;
    FaceMaterial* material[6];
    OGLArrayTexture* texture = nullptr;
    void applyShader(const Shader& shader);
};

class BlockTypeDictionary : public ILoadTask {
public:
    BlockTypeDictionary();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void createBlockType(std::string name, std::string diffuseMapFilename);
    void createBlockType(std::string name, glm::vec4 diffuse);
    BlockType& getBlockTypeAt(int index);
    std::unique_ptr<OGLArrayTexture> arrayTexture;

private:
    const i32 faceTextureSize = 128;
    i32 curLayer = 0;
    std::vector<std::unique_ptr<FaceMaterial>> materials;
    std::vector<std::unique_ptr<BlockType>> blockTypes;


    FaceMaterial* addTexturedMaterialIfNotPresent(std::string filename);
    FaceMaterial* addColoredMaterialIfNotPresent(glm::vec4 color);
};


#endif //GAME_TILETYPEDICTIONARY_H
