//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_ISYSTEM_H
#define CRPG_ISYSTEM_H

#include <memory>
#include <vector>
#include "../defs.h"

class ISystem
{
public:
    ISystem(){}
    virtual ~ISystem(){}
    virtual uint64 getPerformanceFreq()=0;
    virtual uint64 getPerformanceCounter()=0;
    virtual std::shared_ptr<std::vector<uint8>> readFile(std::string path)=0;
    virtual std::string readTextFile(std::string path)=0;
};

#endif //CRPG_ISYSTEM_H
