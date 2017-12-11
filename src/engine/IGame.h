//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IGAME_H
#define CRPG_IGAME_H

#include "IGameMode.h"
#include "video/IRenderer.h"
#include "input/IInput.h"
#include "asset/IAssetLoader.h"
#include "ISystem.h"
#include <string>
#include <memory>

class IGameMode;
class IAssetLoader;

class IGame
{
public:
    IGame(){}
    virtual ~IGame(){}
    virtual void run()=0;
    virtual void registerGameMode(std::shared_ptr<IGameMode> &gm)=0;
    virtual void pushMode(std::string mode)=0;
    virtual void replaceMode(std::string name)=0;
    virtual void popMode()=0;
    virtual std::shared_ptr<IRenderer> getRenderer()=0;
    virtual std::shared_ptr<IInput> getInput()=0;
    virtual std::shared_ptr<IAssetLoader> getAssetLoader()=0;
    virtual std::shared_ptr<ISystem> getSystem()=0;
    virtual double getTime()=0;
    virtual double getDelta()=0;
};
#endif //CRPG_IGAME_H
