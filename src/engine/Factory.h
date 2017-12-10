//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_GAMEFACTORY_H
#define CRPG_GAMEFACTORY_H


#include <memory>
#include "Game.h"

class Factory {
public:
    static std::shared_ptr<IGame> createGame(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IInput> input, std::shared_ptr<IAssetLoader> loader, std::shared_ptr<ISystem> system);

    static std::shared_ptr<ISystem> createSystem();

    // asset
    static std::shared_ptr<IAssetLoader> createAssetLoader();

    // input
    static std::shared_ptr<IInput> createInput();

    // renderer
    static std::shared_ptr<IRenderer> createRenderer();

    // game modes
    static std::shared_ptr<IGameMode> createIntroGameMode(std::shared_ptr<IGame> game);
    static std::shared_ptr<IGameMode> createMenuGameMode(std::shared_ptr<IGame> game);
};


#endif //CRPG_GAMEFACTORY_H
