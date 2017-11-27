//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_IASSETLOADER_H
#define CRPG_IASSETLOADER_H
#include <string>
#include <memory>
#include "IAsset.h"

class IAsset;

class IAssetLoader
{
public:
    IAssetLoader(){}
    virtual ~IAssetLoader(){}
    virtual void clear()=0;
    virtual void add(std::shared_ptr<IAsset> asset)=0;
    virtual void remove(std::shared_ptr<IAsset> asset)=0;
    virtual void load()=0;
    virtual void loadBlocking()=0;
    virtual bool isLoading()=0;
    virtual void update()=0;
    virtual void setGame(std::shared_ptr<IGame> game)=0;
    virtual double getProgress()=0;

    virtual void addLoadTask(std::function<void(std::shared_ptr<IGame>)> const& load, std::function<void(std::shared_ptr<IGame>)> const& prepare)=0;
};
#endif //CRPG_IASSETLOADER_H
