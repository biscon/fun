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

private:
    uint32_t screenWidth;
    uint32_t screenHeight;
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
};


#endif //GAME_OGLRENDERER_H
