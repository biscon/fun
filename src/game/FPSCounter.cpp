//
// Created by bison on 15-10-2017.
//

#include <SDL_log.h>

#include <utility>
#include "FPSCounter.h"

FPSCounter::FPSCounter(std::shared_ptr<ISystem> system) : system(system){
    timerFreq = system->getPerformanceFreq() / 1000; // how many ticks per ms
    timeStamp = system->getPerformanceCounter();
}

void FPSCounter::update() {
    timeDiff = system->getPerformanceCounter() - timeStamp;
    // timediff is bigger than a sec

    if(timeDiff > system->getPerformanceFreq())
    {
        SDL_Log("Time diff is bigger than a sec, rene diff doesn't care");
        timeDiff = system->getPerformanceFreq() / 60;
    }
    fps = 1000.0 / ((double) timeDiff / (double) timerFreq);
    delta =  1 / fps;
    //SDL_Log("time: %u ms, calls per sec = %u, secs = %f", timediff/timer_freq, fps, delta);
    timeStamp = system->getPerformanceCounter();
}

