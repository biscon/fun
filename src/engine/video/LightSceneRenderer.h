//
// Created by bison on 11-12-2017.
//

#ifndef GAME_LIGHTSCENERENDERER_H
#define GAME_LIGHTSCENERENDERER_H


#include <memory>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

class LightSceneRenderer {
public:
    LightSceneRenderer(std::shared_ptr<Camera> camera);
    void render(float screenWidth, float screenHeight, double time);
    void buildBuffers();

private:
    void setupBufferAttributes();

    std::shared_ptr<Camera> camera;
    std::unique_ptr<Mesh> mesh = nullptr;
    std::unique_ptr<Shader> lightingShader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    unsigned int lightVAO;
    glm::vec3 lightPos = {1.2f, 1.0f, 2.0f};
};


#endif //GAME_LIGHTSCENERENDERER_H
