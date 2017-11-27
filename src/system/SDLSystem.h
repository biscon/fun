//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_SDLSYSTEM_H
#define CRPG_SDLSYSTEM_H


#include "../game/ISystem.h"

class SDLSystem : public ISystem {
public:
    uint64 getPerformanceFreq() override;
    uint64 getPerformanceCounter() override;
};


#endif //CRPG_SDLSYSTEM_H
