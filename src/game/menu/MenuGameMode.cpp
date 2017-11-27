//
// Created by bison on 27-09-2017.
//

#include <SDL_log.h>
#include "MenuGameMode.h"

std::string MenuGameMode::getName() {
    return name;
}

bool MenuGameMode::isInit() {
    return initialized;
}

bool MenuGameMode::init() {
    SDL_Log("MenuGameMode::init");
    initialized = true;
    return true;
}

void MenuGameMode::shutdown() {
    SDL_Log("MenuGameMode::shutdown");
}

void MenuGameMode::resume() {
    SDL_Log("MenuGameMode::resume");
}

void MenuGameMode::pause() {
    SDL_Log("MenuGameMode::pause");
}

void MenuGameMode::update() {

}

void MenuGameMode::fixedUpdate() {

}

MenuGameMode::MenuGameMode(std::shared_ptr<IGame> game) : IGameMode(game) {}
