//
// Created by bison on 28-11-2017.
//

#ifndef GAME_ASSETLOADER2_H
#define GAME_ASSETLOADER2_H

#include <list>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
// needed because GCC on windows sucks
#include "../mingw.thread.h"
#include "../../game/asset/IAssetLoader.h"
#include "../mingw.condition_variable.h"
#include "../../game/asset/ILoadTask.h"

class AssetLoader2 : public IAssetLoader {
public:
    AssetLoader2();

    virtual ~AssetLoader2();

    void load() override;
    void loadBlocking() override;
    bool isLoading() override;
    void update() override;
    void setGame(std::shared_ptr<IGame> game) override;
    double getProgress() override;
    void addLoadTask(ILoadTask* task) override;

    void run();

private:
    std::shared_ptr<IGame> game;
    bool newJob = false;
    std::list<ILoadTask*> loadTasks;
    std::atomic<ILoadTask*> prepareTask;
    std::thread thread;
    std::atomic<bool> shouldQuit;
    std::atomic<bool> isReady;
    std::atomic<bool> loading;
    std::mutex lock;
    std::condition_variable newJobCond;
    //std::mutex prepareLock;
    //std::condition_variable prepareCond;

    void loadAsync();
};


#endif //GAME_ASSETLOADER2_H
