//
// Created by bison on 19-11-2017.
//

#ifndef GAME_OGLRENDERER_H
#define GAME_OGLRENDERER_H


#include <SDL.h>
#include "IRenderer.h"
#include "IndexedMesh.h"
#include "OGLTexture.h"
#include "QuadRenderer.h"


class OGLRenderer : public IRenderer {
public:
    bool init(uint32 screenWidth, uint32 screenHeight) override;
    void shutdown() override;
    void fixedUpdate() override;
    void startFrame() override;
    void renderDefaultLoadScreen(double progress) override;
    int32_t getWidth() override;
    int32_t getHeight() override;
    int32_t getRealWidth() override;
    int32_t getRealHeight() override;
    void onViewportChanged(int32_t newWidth, int32_t newHeight) override;

    void setLogicalViewport() override;

    void setRealViewport() override;

private:
    uint32_t screenWidth, realWidth;
    uint32_t screenHeight, realHeight;
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    SDL_Rect viewport{};
};


#endif //GAME_OGLRENDERER_H
