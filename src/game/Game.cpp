//
// Created by bison on 24-09-2017.
//

#include <SDL.h>

#include <utility>
#include "Game.h"

Game::Game(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IInput> input, std::shared_ptr<IAssetLoader> loader,
           std::shared_ptr<ISystem> system) {
    this->assetLoader = std::move(loader);
    this->renderer = std::move(renderer);
    this->input = std::move(input);
    this->system = std::move(system);
}

void Game::run() {
    shouldQuit = false;
    skipTicks = SDL_GetPerformanceFrequency() / TARGET_FPS;
    uint64_t next_game_tick = SDL_GetPerformanceCounter();
    int loops;

    fixedFpsCounter = std::unique_ptr<FPSCounter>(new FPSCounter(system));
    fpsCounter = std::unique_ptr<FPSCounter>(new FPSCounter(system));

    // init input
    input->init();
    input->setQuitEventListener(this);
    input->addKeyboardEventListener(this);

    // init renderer
    renderer->init(1280, 720);

    assetLoader->clear();
    defaultFont = std::make_shared<FontAsset>("font9x14.png", 9, 14);
    assetLoader->add(defaultFont);
    assetLoader->loadBlocking();

    if(!modes.empty())
    {
        std::shared_ptr<IGameMode>& mode = modes.front();
        if(!mode->isInit())
            mode->init();
        mode->resume();
        modeStack.push(mode);
    }

    while(!shouldQuit)
    {
        update();
        fixedUpdate();
        loops = 0;
        while(SDL_GetPerformanceCounter() > next_game_tick && loops < MAX_FRAMESKIP) {
            update();
            next_game_tick += skipTicks;
            loops++;
        }
    }

    // pause running game mode before shutdown
    if(!modeStack.empty())
    {
        modeStack.top()->pause();
    }

    // empty modestack
    while(!modeStack.empty()) modeStack.pop();

    // shutdown all registered & initialized gamemodes
    SDL_Log("Remaining gamemodes: %d", modes.size());
    for(auto i = modes.begin(); i != modes.end(); ++i)
    {
        if((*i)->isInit())
            (*i)->shutdown();
    }
    modes.clear();

    // shutdown renderer
    renderer->shutdown();

    // shutdown input
    input->removeKeyboardEventListener(this);
    input->shutdown();
}

void Game::fixedUpdate() {
    //SDL_Log("Running fixedUpdate");
    if(renderer != nullptr)
        renderer->startFrame();

    if(!assetLoader->isLoading()) {
        if (!modeStack.empty())
            modeStack.top()->fixedUpdate();
    }
    else
    {
        if(renderer != nullptr)
            renderer->renderDefaultLoadScreen(assetLoader->getProgress());
    }
    if(renderer != nullptr) {
        memset(fpsStr, 0, sizeof(fpsStr));
        double fps = fpsCounter->getFps();
        auto vertex_count = renderer->getQuadRenderer().getVertexCount();
        if(fps > 1000)
        {
            sprintf(fpsStr, "RENDERER FPS: %.0f GAME FPS: >1000 VERTS: %d TEXQUADS: %d", fixedFpsCounter->getFps(), vertex_count, vertex_count/4);
        }
        else
        {
            sprintf(fpsStr, "RENDERER FPS: %.0f GAME FPS: %.0f VERTS: %d TEXQUADS: %d", fixedFpsCounter->getFps(), fps, vertex_count, vertex_count/4);
        }
        renderer->renderText(defaultFont, 0, 0, 3, std::string(fpsStr));
        renderer->fixedUpdate();
    }
    fixedFpsCounter->update();
}

void Game::update() {
    //SDL_Delay(5);
    //SDL_Log("Running update");
    input->update();
    assetLoader->update();
    if(!assetLoader->isLoading())
    {
        if (!modeStack.empty())
            modeStack.top()->update();
    }
    fpsCounter->update();
}

void Game::onQuit() {
    shouldQuit = true;
}

void Game::registerGameMode(std::shared_ptr<IGameMode> &gm) {
    modes.push_back(gm);
}

void Game::pushMode(std::string name) {
    if(assetLoader->isLoading()) {
        SDL_Log("AssetLoader working, cannot push game mode");
        return;
    }
    for(auto i = modes.begin(); i != modes.end(); ++i)
    {
        auto mode = *i;
        if(mode->getName() == name)
        {
            SDL_Log("Pushing %s", name.c_str());
            if(!modeStack.empty())
                modeStack.top()->pause();
            if(!mode->isInit())
                mode->init();
            mode->resume();
            modeStack.push(mode);
            return;
        }
    }
}

void Game::replaceMode(std::string name)
{
    if(assetLoader->isLoading()) {
        SDL_Log("AssetLoader working, cannot replace current game mode");
        return;
    }
    for(auto i = modes.begin(); i != modes.end(); ++i)
    {
        auto mode = *i;
        if(mode->getName() == name)
        {
            SDL_Log("Replacing %s", name.c_str());
            if(!modeStack.empty()) {
                modeStack.top()->pause();
                modeStack.pop();
            }
            if(!mode->isInit())
                mode->init();
            mode->resume();
            modeStack.push(mode);
            return;
        }
    }
}

void Game::popMode() {
    if(assetLoader->isLoading()) {
        SDL_Log("AssetLoader working, cannot pop current game mode");
        return;
    }
    if(modeStack.empty())
        return;
    SDL_Log("Popping %s", modeStack.top()->getName().c_str());
    modeStack.top()->pause();
    modeStack.pop();
    if(!modeStack.empty())
    {
        modeStack.top()->resume();
    }
}

void Game::onKeyDown(const SDL_Event *event) {
    if(assetLoader->isLoading()) {
        SDL_Log("AssetLoader working, Ignoring input");
        return;
    }
    if(event->key.keysym.sym == SDLK_ESCAPE)
    {
        if(!modeStack.empty())
            popMode();
        if(modeStack.empty())
            shouldQuit = true;
        return;
    }
}

std::shared_ptr<IRenderer> Game::getRenderer() {
    return renderer;
}

std::shared_ptr<IInput> Game::getInput() {
    return input;
}

std::shared_ptr<IAssetLoader> Game::getAssetLoader() {
    return assetLoader;
}

void Game::initFpsTimer() {

}

std::shared_ptr<ISystem> Game::getSystem() {
    return system;
}


