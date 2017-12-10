//
// Created by bison on 15-10-2017.
//

#include <SDL_timer.h>
#include "SDLSystem.h"

uint64 SDLSystem::getPerformanceFreq() {
    return SDL_GetPerformanceFrequency();
}

uint64 SDLSystem::getPerformanceCounter() {
    return SDL_GetPerformanceCounter();
}
