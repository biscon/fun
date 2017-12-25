//
// Created by bison on 25-12-2017.
//

#ifndef GAME_MODELRENDERER_H
#define GAME_MODELRENDERER_H

#include <memory>
#include <engine/ISystem.h>
#include <array>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Model.h"

class ModelRenderer {
public:
    ModelRenderer(ISystem &system, const std::shared_ptr<Model> &model, const std::shared_ptr<Camera> &camera);
    void render(float screenWidth, float screenHeight, double time);

private:
    ISystem& system;
    std::shared_ptr<Model> model = nullptr;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    unsigned int lightVAO;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};
};


#endif //GAME_MODELRENDERER_H
