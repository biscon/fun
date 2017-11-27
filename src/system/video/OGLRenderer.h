//
// Created by bison on 19-11-2017.
//

#ifndef GAME_OGLRENDERER_H
#define GAME_OGLRENDERER_H


#include <SDL.h>
#include "../../game/video/IRenderer.h"
#include "Shader.h"
#include "RenderMesh.h"
#include "OGLTexture.h"
#include "QuadRenderer.h"


class OGLRenderer : public IRenderer {
public:
    bool init(uint32 screenWidth, uint32 screenHeight) override;
    void shutdown() override;
    void fixedUpdate() override;
    void startFrame() override;
    void renderDefaultLoadScreen(double progress) override;
    TextureManager &getTextureManager() override;
    QuadRenderer &getQuadRenderer() override;

    void renderText(std::shared_ptr<FontAsset> font, float left, float top, int32_t layer, std::string txt) override;

private:
    uint32_t screenWidth;
    uint32_t screenHeight;
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    std::unique_ptr<QuadRenderer> quadRenderer = nullptr;
    std::unique_ptr<TextureManager> textureManager = nullptr;
};


#endif //GAME_OGLRENDERER_H
