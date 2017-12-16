//
// Created by bison on 09-12-2017.
//

#ifndef GAME_FONTRENDERER_H
#define GAME_FONTRENDERER_H


#include <memory>
#include <map>
#include "QuadRenderer.h"
#include "../asset/FontAsset.h"
#include "../asset/ILoadTask.h"

class FontRenderer : public ILoadTask {
public:
    FontRenderer();
    int32_t addFont(std::string filename, uint16_t glyphWidth, uint16_t glyphHeight);
    void render(float screenWidth, float screenHeight);
    void startFrame();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void renderText(int32_t handle, float left, float top, std::string txt);

private:
    std::unique_ptr<QuadRenderer> quadRenderer;
    std::shared_ptr<TextureAtlas> textureAtlas;
    std::map<int32_t, std::unique_ptr<FontAsset>> fonts;
    int32_t nextHandle = 1;

};


#endif //GAME_FONTRENDERER_H
