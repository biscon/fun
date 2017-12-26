//
// Created by bison on 25-12-2017.
//

#ifndef GAME_TILERENDERER_H
#define GAME_TILERENDERER_H

#include <memory>
#include <engine/ISystem.h>
#include <array>
#include <engine/video/Shader.h>
#include <engine/video/Mesh3.h>
#include <engine/video/Camera.h>
#include <engine/video/MaterialDictionary.h>
#include <engine/asset/ILoadTask.h>
#include "TileChunk.h"
#include "TileTypeDictionary.h"

class TileRenderer : public ILoadTask {
public:
    const int VISIBLE_RADIUS = 1;

    TileRenderer(ISystem &system, const std::shared_ptr<Camera> &camera);
    void render(float screenWidth, float screenHeight, double time);

    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void update();

private:
    ISystem& system;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    std::unique_ptr<TileTypeDictionary> tileTypeDict;
    std::unique_ptr<TileChunk> chunk;
    std::vector<std::shared_ptr<TileChunk>> visibleList;
    std::vector<std::shared_ptr<TileChunk>> renderList;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};

    void buildVisibleList();
};


#endif //GAME_TILERENDERER_H
