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

class Game : public IGame, public IQuitEventListener, public IKeyboardEventListener {
public:
    explicit Game(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IInput> input, std::shared_ptr<IAssetLoader> loader, std::shared_ptr<ISystem> system);
    void run() override;
    void onQuit() override;
    void registerGameMode(std::shared_ptr<IGameMode> &gm) override;
    void pushMode(std::string mode) override;
    void replaceMode(std::string name) override;
    void popMode() override;
    void onKeyDown(const SDL_Event *event) override;
    void onKeyUp(const SDL_Event *event) override {}
    void onKeyPressed(const SDL_Event *event) override {}

    std::shared_ptr<IRenderer> getRenderer() override;
    std::shared_ptr<IInput> getInput() override;
    std::shared_ptr<IAssetLoader> getAssetLoader() override;
    std::shared_ptr<ISystem> getSystem() override;

private:
    std::shared_ptr<IRenderer> renderer;
    std::shared_ptr<IInput> input;
    std::shared_ptr<IAssetLoader> assetLoader;
    std::shared_ptr<ISystem> system;
    const uint32 TARGET_FPS = 60;
    const uint32 MAX_FRAMESKIP = 10;
    uint64 skipTicks;
    bool shouldQuit;
    std::stack<std::shared_ptr<IGameMode>> modeStack;
    std::vector<std::shared_ptr<IGameMode>> modes;
    std::shared_ptr<FontAsset> defaultFont;
    std::unique_ptr<FPSCounter> fixedFpsCounter;
    std::unique_ptr<FPSCounter> fpsCounter;
    char fpsStr[256];
    void fixedUpdate();
    void update();
    void initFpsTimer();
};


#endif //CRPG_GAME_H
