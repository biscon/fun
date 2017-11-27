//
// Created by bison on 24-09-2017.
//

#include <SDL.h>
#include "Simulation.h"

void Simulation::update() {
    currentTime = SDL_GetTicks();
    if (currentTime > lastTime + intervalMs) {
        SDL_Log("Tick %d", tick);
        tick++;
        lastTime = currentTime;
    }
}

void Simulation::setRate(uint32 hz) {
    intervalMs = 1000/hz;
}
