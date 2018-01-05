//
// Created by bison on 19-11-2017.
//

#ifndef GAME_OGLRENDERER_H
#define GAME_OGLRENDERER_H


#include <SDL.h>
#include "IRenderer.h"
#include "engine/video/mesh/IndexedMesh.h"
#include "engine/video/texture/OGLTexture.h"
#include "engine/video/ui/QuadRenderer.h"


class Renderer : public IRenderer {
public:
    bool init(u32 screenWidth, u32 screenHeight) override;
    void shutdown() override;
    void fixedUpdate() override;
    void startFrame() override;
    void renderDefaultLoadScreen(double progress) override;
    i32 getWidth() override;
    i32 getHeight() override;
    i32 getRealWidth() override;
    i32 getRealHeight() override;
    void onViewportChanged(i32 newWidth, i32 newHeight) override;
    void setLogicalViewport() override;
    void setRealViewport() override;
    void setWindowBordered(bool bordered) override;
    void maximizeWindow() override;
    void minimizeWindow() override;
    void restoreWindow() override;

private:
    u32 screenWidth, realWidth;
    u32 screenHeight, realHeight;
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    SDL_Rect viewport{};
};


#endif //GAME_OGLRENDERER_H
