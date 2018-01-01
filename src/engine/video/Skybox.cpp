//
// Created by bison on 01-01-2018.
//

#include "Skybox.h"

Skybox::Skybox(ISystem &system, const std::shared_ptr<Camera> &camera) : system(system), camera(camera) {
    auto lighting_vs = system.readTextFile("shaders/skybox_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/skybox_shader_fs.glsl");
    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    mesh = std::unique_ptr<Mesh3>(new Mesh3());
    mesh->generateCubeAt(0,0,0);
}

void Skybox::render(glm::mat4 view, glm::mat4 projection) {
    glm::mat4 model;
    shader->use();
    shader->setMat4("projection", projection);

    view[3][0] = 0.0f;
    view[3][1] = 0.0f;
    view[3][2] = 0.0f;
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    mesh->draw(*shader);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
}

void Skybox::prepare() {
    mesh->prepare();
}
