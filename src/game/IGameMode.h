//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IGAMEMODE_H
#define CRPG_IGAMEMODE_H
#include <string>
#include <memory>
#include <utility>
#include "IGame.h"

// forward declaration
class IGame;

class IGameMode
{
public:
    explicit IGameMode(std::shared_ptr<IGame> game) : game(std::move(game)) {}
    virtual ~IGameMode(){}
    virtual std::string getName()=0;
    virtual bool isInit()=0;
    virtual bool init()=0;
    virtual void shutdown()=0;
    virtual void resume()=0;
    virtual void pause()=0;
    virtual void update()=0;
    virtual void fixedUpdate()=0;

protected:
    std::shared_ptr<IGame> game;
};
#endif //CRPG_IGAMEMODE_H
