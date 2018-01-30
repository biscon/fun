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
#include <engine/video/shader/Shader.h>
#include <engine/video/texture/OGLArrayTexture.h>

struct BlockType
{
    std::string name;
    FaceMaterial material;
    OGLArrayTexture* texture = nullptr;
    void applyShader(const Shader& shader);

    static i32 colorsUniformLocation;
    static i32 layersUniformLocation;
};

__forceinline void BlockType::applyShader(const Shader& shader)
{
    glUniform4fv(colorsUniformLocation, 6, &material.colors[0][0]);
    glUniform1iv(layersUniformLocation, 6, &material.layers[0]);

    //glUniform4fv(2, 6, &material.colors[0][0]);
    //glUniform1iv(11, 6, &material.layers[0]);
}



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
    //std::vector<std::unique_ptr<FaceMaterial>> materials;
    std::map<std::string, i32> textureLayerMap;
    std::map<i32, std::shared_ptr<PixelBuffer>> layerBufferMap;
    std::vector<std::unique_ptr<BlockType>> blockTypes;
    int32_t addLayerIfNotPresent(std::string filename);
};

#endif //GAME_TILETYPEDICTIONARY_H
