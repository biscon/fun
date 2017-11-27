//
// Created by bison on 21-11-2017.
//

#ifndef GAME_TEXTUREMANAGER_H
#define GAME_TEXTUREMANAGER_H

#include <memory>
#include <vector>
#include <map>
#include "../../game/video/TextureAtlas.h"
#include "../../game/video/Texture.h"
#include "OGLTexture.h"

class TextureManager {
public:
    int32_t createTexture(std::shared_ptr<PixelBuffer> pb);
    void buildTextures();
    void uploadTextures();
    void bind(int32_t handle);
    void bindAtlas(TextureAtlas* atlas);
    UVRect* getUVRect(int32_t handle);
    Texture* getTexture(int32_t handle);
    std::vector<std::unique_ptr<TextureAtlas>>& getAtlases();

private:
    const uint32_t ATLAS_SIZE = 1024;
    int32_t nextHandle = 1;
    std::map<int32_t, std::unique_ptr<Texture>> handles;
    std::vector<std::unique_ptr<TextureAtlas>> atlases;
    std::vector<std::unique_ptr<OGLTexture>> textures;
    std::map<TextureAtlas*, OGLTexture*> atlasTextureMap;
};


#endif //GAME_TEXTUREMANAGER_H
