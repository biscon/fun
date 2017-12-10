//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_ISYSTEM_H
#define CRPG_ISYSTEM_H

#include "../defs.h"

class ISystem
{
public:
    ISystem(){}
    virtual ~ISystem(){}
    virtual uint64 getPerformanceFreq()=0;
    virtual uint64 getPerformanceCounter()=0;
};

#endif //CRPG_ISYSTEM_H
