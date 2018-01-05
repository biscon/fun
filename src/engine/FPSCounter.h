//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_FPSCOUNTER_H
#define CRPG_FPSCOUNTER_H

#include <memory>
#include "../defs.h"
#include "ISystem.h"

class FPSCounter {
public:
    explicit FPSCounter(std::shared_ptr<ISystem> system);
    void update();

    double getFps() const {
        return fps;
    }

    double getDelta() const {
        return delta;
    }

private:
    std::shared_ptr<ISystem> system;
    double fps = 0;
    u64 timerFreq = 0;
    u64 timeStamp = 0;
    u64 timeDiff = 0;
    double delta = 0;
};


#endif //CRPG_FPSCOUNTER_H
