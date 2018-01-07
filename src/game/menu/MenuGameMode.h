//
// Created by bison on 27-09-2017.
//

#ifndef CRPG_MENUGAMEMODE_H
#define CRPG_MENUGAMEMODE_H


#include "../../engine/IGameMode.h"

class MenuGameMode : public IGameMode {
public:
    MenuGameMode(std::shared_ptr<IGame> game);

    std::string getName() override;
    bool isInit() override;
    bool init() override;
    void shutdown() override;
    void resume() override;
    void pause() override;
    void update(double delta) override;
    void render(double delta) override;

private:
    bool initialized = false;
    std::string name = "MenuGameMode";
};


#endif //CRPG_MENUGAMEMODE_H
