//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_SIMULATION_H
#define CRPG_SIMULATION_H

#include "../defs.h"

class Simulation {
public:
    void update();
    void setRate(u32 hz);

private:
    u32 lastTime = 0, currentTime, intervalMs = 1000, tick = 0;
};


#endif //CRPG_SIMULATION_H
