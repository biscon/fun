//
// Created by bison on 09-12-2017.
//

#ifndef GAME_FONTRENDERER_H
#define GAME_FONTRENDERER_H


#include <memory>
#include <map>
#include "engine/video/ui/QuadRenderer.h"
#include "engine/asset/FontAsset.h"
#include "engine/asset/ILoadTask.h"

class FontRenderer : public ILoadTask {
public:
    FontRenderer();
    i32 addFont(std::string filename, uint16_t glyphWidth, uint16_t glyphHeight);
    void render(float screenWidth, float screenHeight);
    void startFrame();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void renderText(i32 handle, float left, float top, std::string txt);

private:
    std::unique_ptr<QuadRenderer> quadRenderer;
    std::shared_ptr<TextureAtlas> textureAtlas;
    std::map<i32, std::unique_ptr<FontAsset>> fonts;
    i32 nextHandle = 1;

};


#endif //GAME_FONTRENDERER_H
