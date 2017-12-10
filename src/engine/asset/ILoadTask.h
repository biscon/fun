//
// Created by bison on 27-11-2017.
//

#ifndef GAME_ILOADTASK_H
#define GAME_ILOADTASK_H

#include "../IGame.h"

class IGame;

class ILoadTask
{
public:
    // gets called from the loader thread
    virtual bool load(IGame& game)=0;
    // gets called from the main thread (owning the gl context)
    virtual bool prepare(IGame& game)=0;
};
#endif //GAME_ILOADTASK_H
