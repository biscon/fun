//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_GAME_H
#define CRPG_GAME_H

#include <stack>
#include <vector>
#include <string>
#include <memory>
#include "IGameMode.h"
#include "IGame.h"
#include "../defs.h"
#include "input/IKeyboardEventListener.h"
#include "input/IInput.h"
#include "video/IRenderer.h"
#include "asset/FontAsset.h"
#include "ISystem.h"
#include "FPSCounter.h"
#include "video/FontRenderer.h"

class Game : public IGame, public IQuitEventListener, public IKeyboardEventListener, public IWindowEventListener {
public:
    explicit Game(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IInput> input, std::shared_ptr<IAssetLoader> loader, std::shared_ptr<ISystem> system);
    void run() override;
    void onQuit() override;
    void registerGameMode(std::shared_ptr<IGameMode> &gm) override;
    void pushMode(std::string mode) override;
    void replaceMode(std::string name) override;
    void popMode() override;

    // keyboard events
    void onKeyDown(const SDL_Event *event) override;
    void onKeyUp(const SDL_Event *event) override {}
    void onKeyPressed(const SDL_Event *event) override {}

    // window events
    void onWindowResize(int32_t width, int32_t height) override;
    void onMouseEnter() override;
    void onMouseLeave() override;
    void onFocusGained() override;
    void onFocusLost() override;

    std::shared_ptr<IRenderer> getRenderer() override;
    std::shared_ptr<IInput> getInput() override;
    std::shared_ptr<IAssetLoader> getAssetLoader() override;
    std::shared_ptr<ISystem> getSystem() override;
    double getTime() override;
    double getDelta() override;

private:
    std::shared_ptr<IRenderer> renderer;
    std::shared_ptr<IInput> input;
    std::shared_ptr<IAssetLoader> assetLoader;
    std::shared_ptr<ISystem> system;
    const uint32 TARGET_FPS = 100;
    const uint32 MAX_FRAMESKIP = 10;
    uint64 skipTicks;
    bool shouldQuit;
    std::stack<std::shared_ptr<IGameMode>> modeStack;
    std::vector<std::shared_ptr<IGameMode>> modes;
    std::shared_ptr<FontRenderer> defaultFontRenderer;
    int32_t defaultFont = -1;
    std::unique_ptr<FPSCounter> fixedFpsCounter;
    std::unique_ptr<FPSCounter> fpsCounter;
    char fpsStr[256];
    void fixedUpdate();
    void update();
    void initFpsTimer();
    uint64 initTimeStamp = 0;
    int32 screenWidth;
    int32 screenHeight;
    bool paused = false;
    bool fullscreen = false;
};


#endif //CRPG_GAME_H
