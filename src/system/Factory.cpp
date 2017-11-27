//
// Created by bison on 24-09-2017.
//

#include "Factory.h"
#include "SDLSystem.h"
#include "asset/AssetLoader.h"
#include "input/SDLInput.h"
#include "video/OGLRenderer.h"
#include "../game/menu/MenuGameMode.h"
#include "../game/intro/IntroGameMode.h"

std::shared_ptr<IGame> Factory::createGame(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IInput> input, std::shared_ptr<IAssetLoader> loader, std::shared_ptr<ISystem> system) {
    return std::make_shared<Game>(renderer, input, loader, system);
}

std::shared_ptr<ISystem> Factory::createSystem() {
    return std::make_shared<SDLSystem>();
}

std::shared_ptr<IAssetLoader> Factory::createAssetLoader() {
    return std::make_shared<AssetLoader>();
}

std::shared_ptr<IInput> Factory::createInput() {
    return std::make_shared<SDLInput>();
}

std::shared_ptr<IRenderer> Factory::createRenderer() {
    return std::make_shared<OGLRenderer>();
}

std::shared_ptr<IGameMode> Factory::createIntroGameMode(std::shared_ptr<IGame> game) {
    return std::make_shared<IntroGameMode>(game);
}

std::shared_ptr<IGameMode> Factory::createMenuGameMode(std::shared_ptr<IGame> game) {
    return std::make_shared<MenuGameMode>(game);
}
