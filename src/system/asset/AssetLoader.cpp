//
// Created by bison on 01-10-2017.
//

#include <SDL_log.h>
#include "AssetLoader.h"
#include "../../defs.h"
#include "../../game/IGame.h"

LoadTask::LoadTask(std::function<void(IGame*)> const& load,
                   std::function<void(IGame*)> const& prepare) : load(load), prepare(prepare) {}


static int LoaderThread(void *ptr)
{
    SDL_Log("AssetLoaderThread:: spinning up");
    //auto loader = dynamic_cast<AssetLoader*>(ptr);
    auto loader = (AssetLoader*) ptr;
    if(loader == nullptr)
    {
        SDL_Log("AssetLoaderThread: Couldn't acquire pointer to asset loader");
        return -1;
    }

    while(true)
    {
        SDL_Log("AssetLoaderThread: Taking a nap zZzz...");
        SDL_LockMutex(loader->newJobLock);
        while (!loader->hasNewJob) {
            SDL_CondWait(loader->newJobCond, loader->newJobLock);
        }
        SDL_Log("AssetLoaderThread: Waking up");
        SDL_UnlockMutex(loader->newJobLock);
        if(loader->shouldQuit)
            break;
        if(loader->hasNewJob)
        {
            loader->progress = 0;
            if(loader->loadTasks.empty())
            {
                SDL_Log("AssetLoaderThread: Found no assets to load, going back too sleep");
                SDL_LockMutex(loader->newJobLock);
                loader->hasNewJob = false;
                loader->loading = false;
                SDL_UnlockMutex(loader->newJobLock);
                continue;
            }
            uint32 count = 0;
            uint32 total = (uint32) loader->loadTasks.size();
            loader->game->getRenderer();
            for(auto i = loader->loadTasks.begin(); i != loader->loadTasks.end(); i++)
            {
                SDL_Log("About to crash");
                i->get()->load(loader->game.get());


                if(!i->get()->isPrepared)
                {
                    // goto sleep waiting for mainthread to prepare asset
                    //SDL_Log("AssetLoaderThread: Taking a nap while mainthread prepares asset zZzz...");
                    SDL_LockMutex(loader->prepareLock);
                    loader->prepareTask = i->get();
                    while (loader->prepareTask != nullptr) {
                        SDL_Log("CondWaiting");
                        SDL_CondWait(loader->prepareCond, loader->prepareLock);
                    }
                    //SDL_Log("AssetLoaderThread: Mainthread done preparing, continuing...");
                    SDL_UnlockMutex(loader->prepareLock);
                }
                count++;
                loader->progress = (double) count / (double) total;
                //SDL_Log("Progress updated to %.2f", loader->progress);
            }
            SDL_LockMutex(loader->newJobLock);
            loader->hasNewJob = false;
            SDL_UnlockMutex(loader->newJobLock);
            SDL_Log("AssetLoaderThread: Done executing %d load tasks", loader->loadTasks.size());
            loader->loadTasks.clear();
        }
    }
    SDL_Log("AssetLoaderThread: Terminating");
    return 0;
}

AssetLoader::AssetLoader() {
    newJobLock = SDL_CreateMutex();
    newJobCond = SDL_CreateCond();
    prepareLock = SDL_CreateMutex();
    prepareCond = SDL_CreateCond();
    thread = SDL_CreateThread(LoaderThread, "LoaderThread", (void *) this);

}

AssetLoader::~AssetLoader() {
    int status = 0;

    SDL_LockMutex(newJobLock);
    hasNewJob = true;
    shouldQuit = true;
    SDL_CondSignal(newJobCond);
    SDL_UnlockMutex(newJobLock);
    SDL_WaitThread(thread, &status);


    SDL_DestroyCond(newJobCond);
    SDL_DestroyMutex(newJobLock);
    SDL_DestroyCond(prepareCond);
    SDL_DestroyMutex(prepareLock);
}

bool AssetLoader::isLoading() {
    return loading;
}

void AssetLoader::update() {
    if(loading) {
        // asset ready to prepare
        if (prepareTask != nullptr) {
            SDL_Log("AssetLoader: MainThread preparing task");
            prepareTask->prepare(game.get());
            prepareTask->isPrepared = true;
            // wake up loader thread
            SDL_LockMutex(prepareLock);
            prepareTask = nullptr;
            SDL_CondSignal(prepareCond);
            SDL_UnlockMutex(prepareLock);
        }
    }
}

void AssetLoader::setGame(std::shared_ptr<IGame> game) {
    this->game = game;
}

double AssetLoader::getProgress() {
    return progress;
}

void AssetLoader::load() {
    if(loadTasks.empty())
    {
        SDL_Log("AssetLoader: Can't load, no assets");
        return;
    }
    SDL_Log("AssetLoader: Loading...");
    loading = true;
    // wake up loader thread
    SDL_LockMutex(newJobLock);
    hasNewJob = true;
    SDL_CondSignal(newJobCond);
    SDL_UnlockMutex(newJobLock);
}

void AssetLoader::loadBlocking() {
    if(loadTasks.empty())
    {
        SDL_Log("AssetLoader: Can't load, no assets");
        return;
    }
    SDL_Log("AssetLoader: Loading synchronously...");
    for(auto i = loadTasks.begin(); i != loadTasks.end(); i++)
    {
        i->get()->load(game.get());
        i->get()->prepare(game.get());
    }
    SDL_Log("AssetLoader: Done executing %d load tasks", loadTasks.size());
    loadTasks.clear();
}

/*
void AssetLoader::addLoadTask(std::function<void(IGame*)> const &load,
                              std::function<void(IGame*)> const &prepare) {
    loadTasks.push_back(std::unique_ptr<LoadTask>(new LoadTask(load, prepare)));
}
 */




