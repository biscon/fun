//
// Created by bison on 11-12-2017.
//

#include "LightSceneRenderer.h"
#include "shaders.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

LightSceneRenderer::LightSceneRenderer(std::shared_ptr<Camera> camera) : camera(camera) {
    lightingShader = std::unique_ptr<Shader>(new Shader(lightingShaderVertexSrc, lightingShaderFragmentSrc, nullptr));
    lampShader = std::unique_ptr<Shader>(new Shader(lampShaderVertexSrc, lampShaderFragmentSrc, nullptr));
}

void LightSceneRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);
    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader->use();
    lightingShader->setVec3("objectColor", 1.0f, 0.843137f, 0.0f);
    lightingShader->setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    lightingShader->setVec3("lightPos", lightPos);
    lightingShader->setVec3("viewPos", camera->Position);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    // world transformation
    glm::mat4 model;
    model = glm::rotate(model, (float)(time) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    lightingShader->setMat4("model", model);

    // render the cube
    mesh->bindVAO();
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // also draw the lamp object
    lampShader->use();
    lampShader->setMat4("projection", projection);
    lampShader->setMat4("view", view);
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    lampShader->setMat4("model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDisable(GL_DEPTH_TEST);
}

void LightSceneRenderer::buildBuffers() {
    mesh = std::unique_ptr<Mesh>(new Mesh(MeshUpdateType::STATIC));
    mesh->bindVAO();
    mesh->bindVBO();
    mesh->generateCube();
    mesh->upload();
    setupBufferAttributes();
}

void LightSceneRenderer::setupBufferAttributes() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("setup OpenGL error: %d", err);
    }
    // bind index 0 to position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // bind index 1 to normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // setup light vao
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // bind mesh again
    mesh->bindVBO();
    // bind index 0 to position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


