//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_SDLSYSTEM_H
#define CRPG_SDLSYSTEM_H

#include <string>
#include "ISystem.h"

class SDLSystem : public ISystem {
public:
    u64 getPerformanceFreq() override;
    u64 getPerformanceCounter() override;
    std::shared_ptr<std::vector<u8>> readFile(std::string path) override;
    std::string readTextFile(std::string path) override;
};


#endif //CRPG_SDLSYSTEM_H
