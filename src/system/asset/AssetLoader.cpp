//
// Created by bison on 01-10-2017.
//

#include <SDL_log.h>
#include "AssetLoader.h"
#include "../../defs.h"
#include "../../game/IGame.h"

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
            if(loader->assets.empty())
            {
                SDL_Log("AssetLoaderThread: Found no assets to load, going back too sleep");
                SDL_LockMutex(loader->newJobLock);
                loader->hasNewJob = false;
                loader->loading = false;
                SDL_UnlockMutex(loader->newJobLock);
                continue;
            }
            uint32 count = 0;
            uint32 total = (uint32) loader->assets.size();
            for(auto i = loader->assets.begin(); i != loader->assets.end(); i++)
            {
                auto asset = *i;
                if(asset->getStatus() == AssetStatus::UNLOADED) {
                    if(!asset->load(*loader->game))
                    {
                        SDL_Log("AssetLoaderThread: Failed load asset %s", asset->getName().c_str());
                    }
                    else {
                        SDL_Log("AssetLoaderThread: Loaded asset %s", asset->getName().c_str());
                        asset->setStatus(AssetStatus::LOADED);
                    }
                }
                if(asset->getStatus() == AssetStatus::LOADED)
                {
                    // goto sleep waiting for mainthread to prepare asset
                    //SDL_Log("AssetLoaderThread: Taking a nap while mainthread prepares asset zZzz...");
                    SDL_LockMutex(loader->prepareLock);
                    loader->prepareAsset = &*asset;
                    while (loader->prepareAsset != nullptr) {
                        SDL_CondWait(loader->prepareCond, loader->prepareLock);
                    }
                    //SDL_Log("AssetLoaderThread: Mainthread done preparing, continuing...");
                    SDL_UnlockMutex(loader->prepareLock);
                }
                count++;
                loader->progress = (double) count / (double) total;
                //SDL_Log("Progress updated to %.2f", loader->progress);
            }
            loader->game->getRenderer()->getTextureManager().buildTextures();
            loader->shouldUploadTextures = true;
            SDL_LockMutex(loader->newJobLock);
            loader->hasNewJob = false;
            SDL_UnlockMutex(loader->newJobLock);
            SDL_Log("AssetLoaderThread: Done loading %d assets", loader->assets.size());
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
        if (prepareAsset != nullptr) {
            SDL_Log("AssetLoader: MainThread preparing asset %s", prepareAsset->getName().c_str());
            if (!prepareAsset->prepare(*game)) {
                SDL_Log("AssetLoader: Failed to prepare asset %s", prepareAsset->getName().c_str());
            } else
                prepareAsset->setStatus(AssetStatus::READY);
            // wake up loader thread
            SDL_LockMutex(prepareLock);
            prepareAsset = nullptr;
            SDL_CondSignal(prepareCond);
            SDL_UnlockMutex(prepareLock);
        }
        if(shouldUploadTextures)
        {
            shouldUploadTextures = false;
            loading = false;
            game->getRenderer()->getTextureManager().uploadTextures();
            game->getRenderer()->getQuadRenderer().buildBuffers();
        }
    }
}

void AssetLoader::setGame(std::shared_ptr<IGame> game) {
    this->game = game;
}

double AssetLoader::getProgress() {
    return progress;
}

void AssetLoader::clear() {
    assets.clear();
}

void AssetLoader::add(std::shared_ptr<IAsset> asset) {
    assets.push_back(asset);
}

void AssetLoader::remove(std::shared_ptr<IAsset> asset) {
    assets.remove(asset);
}

void AssetLoader::load() {
    if(assets.empty())
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
    if(assets.empty())
    {
        SDL_Log("AssetLoader: Can't load, no assets");
        return;
    }
    SDL_Log("AssetLoader: Loading synchronously...");
    for(auto i = assets.begin(); i != assets.end(); i++)
    {
        auto asset = *i;
        if(asset->getStatus() == AssetStatus::UNLOADED) {
            if(!asset->load(*game))
            {
                SDL_Log("AssetLoader: Failed to load asset %s", asset->getName().c_str());
            }
            else {
                SDL_Log("AssetLoader Loaded asset %s", asset->getName().c_str());
                asset->setStatus(AssetStatus::LOADED);
            }
        }
        if(asset->getStatus() == AssetStatus::LOADED)
        {
            if(!asset->prepare(*game))
            {
                SDL_Log("AssetLoader: Failed to prepare asset %s", asset->getName().c_str());
            }
            else {
                SDL_Log("AssetLoader: Prepared asset %s", asset->getName().c_str());
                asset->setStatus(AssetStatus::READY);
            }
        }
    }
    game->getRenderer()->getTextureManager().buildTextures();
    game->getRenderer()->getTextureManager().uploadTextures();
    game->getRenderer()->getQuadRenderer().buildBuffers();
    SDL_Log("AssetLoader: Done loading %d assets", assets.size());
}

void AssetLoader::addLoadTask(std::function<void(std::shared_ptr<IGame>)> const &load,
                              std::function<void(std::shared_ptr<IGame>)> const &prepare) {
    load(game);
}

/*
void AssetLoader::unload() {
    if(isLoading())
    {
        SDL_Log("Can't unload while loading");
        return;
    }
    if(assets.empty())
    {
        SDL_Log("AssetLoader: Can't unload, no assets");
        return;
    }
    SDL_Log("AssetLoader: Unloading assets");
    for(auto i = assets.begin(); i != assets.end(); i++)
    {
        auto asset = *i;
        if(asset->getStatus() == AssetStatus::READY)
        {
            if(!asset->unprepare(*game))
            {
                SDL_Log("AssetLoader: Unpreparing of asset %s failed", asset->getName().c_str());
            }
            else
                asset->setStatus(AssetStatus::LOADED);
        }
        if(asset->getStatus() == AssetStatus::LOADED)
        {
            if(!asset->unload(*game))
            {
                SDL_Log("AssetLoader: Unloading of asset %s failed", asset->getName().c_str());
            }
            else
                asset->setStatus(AssetStatus::UNLOADED);
        }
    }
    assets.clear();
    SDL_Log("AssetLoader: Done unloading %d assets", assets.size());
}
 */
