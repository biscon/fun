//
// Created by bison on 28-11-2017.
//

#include <SDL_log.h>
#include "AssetLoader.h"

AssetLoader::AssetLoader() {
    loading = false;
    thread = std::thread(AssetLoader::run, this);
}

AssetLoader::~AssetLoader() {
    SDL_Log("AssetLoader: shutting down...");
    shouldQuit = true;
    std::unique_lock<std::mutex> lk(lock);
    newJob = true;
    shouldQuit = true;
    newJobCond.notify_one();
    lk.unlock();
    thread.join();
}

void AssetLoader::load() {
    // wait for thread to initialize
    loading = true;
    prepareTask = nullptr;
    while(!isReady)
    {
        SDL_Log("waiting for assetloaderthread");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    //SDL_Log("locking");
    std::unique_lock<std::mutex> lk(lock);
    //SDL_Log("afterlocking");
    newJob = true;
    //SDL_Log("notifying nj = %d", newJob);
    newJobCond.notify_one();
    lk.unlock();
}

void AssetLoader::loadBlocking() {
    SDL_Log("AssetLoader: Loading synchronously...");
    for (auto &loadTask : loadTasks) {
        loadTask->load(*game.get());
        loadTask->prepare(*game.get());
    }
    SDL_Log("AssetLoader: Done executing %d load tasks", loadTasks.size());
    loadTasks.clear();
}

bool AssetLoader::isLoading() {
    return loading;
}

void AssetLoader::update() {
    if(prepareTask != nullptr)
    {
        ILoadTask* task = prepareTask;
        task->prepare(*game.get());
        prepareTask = nullptr;
    }
}

void AssetLoader::setGame(std::shared_ptr<IGame> game) {
    this->game = game;
}

double AssetLoader::getProgress() {
    return 0;
}

void AssetLoader::addLoadTask(ILoadTask* task) {
    loadTasks.push_back(task);
}

void AssetLoader::loadAsync()
{
    SDL_Log("AssetLoaderThread: Loading asynchronously...");
    for (auto &loadTask : loadTasks) {
        loadTask->load(*game.get());
        prepareTask = loadTask;
        // wait for mainthread to prepare
        //SDL_Log("Waiting for mainthread to prepare");
        while(prepareTask != nullptr)
        {
            //SDL_Log("waiting");
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::this_thread::yield();
        }
        //SDL_Log("Preparation done");
    }
    SDL_Log("AssetLoaderThread: Done executing %d load tasks", loadTasks.size());
    loadTasks.clear();
}

void AssetLoader::run() {
    shouldQuit = false;
    isReady = false;
    std::unique_lock<std::mutex> l(lock);
    newJob = false;
    l.unlock();
    while(true) {
        SDL_Log("AssetLoaderThread: Ready, waiting for work... nj = %d", newJob);
        //SDL_Log("t locking");
        std::unique_lock<std::mutex> lk(lock);
        //SDL_Log("t after locking");
        isReady = true;
        while (!newJob)
        {
            newJobCond.wait(lk);
            if (!newJob)
                SDL_Log("Spurious wake up!");
        }
        lk.unlock();
        SDL_Log("AssetLoaderThread: woke up, nj = %d", newJob);
        loadAsync();
        std::unique_lock<std::mutex> l2(lock);
        newJob = false;
        loading = false;
        l2.unlock();
        if(shouldQuit)
            break;
    }
    SDL_Log("AssetLoaderThread: Quitting...");
}



