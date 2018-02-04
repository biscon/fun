//
// Created by bison on 04-02-2018.
//

#ifndef GAME_SCENERYMANAGER_H
#define GAME_SCENERYMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <condition_variable>
// needed because GCC on windows sucks
#ifdef __WIN32__
#include <engine/mingw.thread.h>
#include <engine/mingw.condition_variable.h>
#include <engine/mingw.mutex.h>
#endif

#include "ChunkBlockPos.h"

struct SceneryType {
    std::string name;
    std::string textureFilename;
};

struct SceneryInstance {
    SceneryType *type;
    u8 lightLevel;
};


class SceneryManager {
public:
    std::mutex activeLock;
    std::vector<std::unique_ptr<SceneryType>> sceneryTypes;
    std::vector<std::unique_ptr<SceneryInstance>> recycledScenery;
    std::unordered_map<ChunkBlockPos, std::unique_ptr<SceneryInstance>> activeScenery;

    void load();
    void prepare();

private:

};


#endif //GAME_SCENERYMANAGER_H
