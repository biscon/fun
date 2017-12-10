//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_IASSETLOADER_H
#define CRPG_IASSETLOADER_H
#include <string>
#include <memory>
#include "ILoadTask.h"

class ILoadTask;
class IGame;
class IAssetLoader
{
public:
    IAssetLoader(){}
    virtual ~IAssetLoader(){}
    virtual void load()=0;
    virtual void loadBlocking()=0;
    virtual bool isLoading()=0;
    virtual void update()=0;
    virtual void setGame(std::shared_ptr<IGame> game)=0;
    virtual double getProgress()=0;
    virtual void addLoadTask(ILoadTask* task)=0;
};
#endif //CRPG_IASSETLOADER_H
