//
// Created by bison on 01-01-2018.
//

#ifndef GAME_SKYBOX_H
#define GAME_SKYBOX_H

#include <memory>
#include <glm/glm.hpp>
#include <engine/system/ISystem.h>
#include "engine/video/shader/Shader.h"
#include "engine/video/mesh/Mesh3.h"
#include "engine/video/camera/Camera.h"

class Skybox {
public:
    Skybox(ISystem &system, const std::shared_ptr<Camera> &camera);
    void render(glm::mat4 view, glm::mat4 projection, float intensity);
    void prepare();

private:
    ISystem& system;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Mesh3> mesh;
    std::unique_ptr<Shader> shader = nullptr;

};


#endif //GAME_SKYBOX_H
