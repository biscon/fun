//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_ASSETLOADER_H
#define CRPG_ASSETLOADER_H

#include <string>
#include <list>
#include <map>
#include <SDL.h>
#include <memory>
#include "../../game/asset/IAsset.h"
#include "../../game/asset/IAssetLoader.h"

class AssetTag {
public:
    std::string tag;

};

struct LoadTask {
    LoadTask(std::function<void(IGame*)> const &load, std::function<void(IGame*)> const &prepare);
    std::function<void(IGame*)> const& load;
    std::function<void(IGame*)> const& prepare;
    bool isPrepared = false;
};

class AssetLoader : public IAssetLoader {
public:
    AssetLoader();
    virtual ~AssetLoader();
    bool isLoading() override;
    void update() override;
    void setGame(std::shared_ptr<IGame> game) override;
    double getProgress() override;
    void load() override;
    void loadBlocking() override;
    //void addLoadTask(std::function<void(IGame*)> const &load, std::function<void(IGame*)> const &prepare) override;

    SDL_Thread *thread;
    SDL_mutex *prepareLock;
    SDL_cond *prepareCond;
    SDL_mutex *newJobLock;
    SDL_cond *newJobCond;
    bool hasNewJob = false;
    bool shouldQuit = false;
    bool loading = false;
    LoadTask* prepareTask = nullptr;
    double progress = 0;
    std::shared_ptr<IGame> game;
    //std::list<std::shared_ptr<IAsset>> assets;
    std::list<std::unique_ptr<LoadTask>> loadTasks;

private:
};


#endif //CRPG_ASSETLOADER_H
