//
// Created by bison on 11-12-2017.
//

#ifndef GAME_LIGHTSCENERENDERER_H
#define GAME_LIGHTSCENERENDERER_H


#include <memory>
#include <engine/system/ISystem.h>
#include <array>
#include "engine/video/shader/Shader.h"
#include "engine/video/mesh/Mesh.h"
#include "engine/video/camera/Camera.h"
#include "engine/video/model/Model.h"

class LightSceneRenderer {
public:
    LightSceneRenderer(ISystem& system, std::shared_ptr<Camera> camera);
    void render(float screenWidth, float screenHeight, double time);
    void buildBuffers(std::shared_ptr<OGLTexture> diffuse_map, std::shared_ptr<OGLTexture> specular_map);

    std::shared_ptr<Model> testModel;

private:
    void setupBufferAttributes();

    std::shared_ptr<Camera> camera;
    std::unique_ptr<Mesh> mesh = nullptr;
    std::unique_ptr<Shader> lightingShader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    unsigned int lightVAO;
    glm::vec3 lightPos = {1.2f, 1.0f, 2.0f};
    ISystem& system;
    std::shared_ptr<OGLTexture> diffuseMap;
    std::shared_ptr<OGLTexture> specularMap;
    std::array<glm::vec3, 10> cubePositions;
    std::array<glm::vec3, 4> pointLightPositions;
};


#endif //GAME_LIGHTSCENERENDERER_H
