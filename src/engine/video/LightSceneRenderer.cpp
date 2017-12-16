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

LightSceneRenderer::LightSceneRenderer(ISystem& system, std::shared_ptr<Camera> camera) : camera(camera), system(system) {
    auto lighting_vs = system.readTextFile("shaders/lighting_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/lighting_shader_fs.glsl");
    lightingShader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    auto lamp_vs = system.readTextFile("shaders/lamp_shader_vs.glsl");
    auto lamp_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    lampShader = std::unique_ptr<Shader>(new Shader(lamp_vs.c_str(), lamp_fs.c_str(), nullptr));

    cubePositions = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };
}

void LightSceneRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);
    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader->use();

    diffuseMap->bind(0);
    specularMap->bind(1);

    lightingShader->setVec3("viewPos", camera->Position);

    lightingShader->setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    lightingShader->setVec3("light.diffuse",  0.8f, 0.8f, 0.8f); // darken the light a bit to fit the scene
    lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShader->setVec3("light.position", lightPos);

    // set material
    lightingShader->setInt("material.diffuse", 0);
    lightingShader->setInt("material.specular", 1);
    lightingShader->setFloat("material.shininess", 32.0f);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();

    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    // world transformation
/*
    glm::mat4 model;
    model = glm::rotate(model, (float)(time) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    lightingShader->setMat4("model", model);
*/
    glm::mat4 model;
    // render the cube
    mesh->bindVAO();
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    for(unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model;
        model = glm::translate(model, cubePositions[i]);
        //model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // also draw the lamp object
    lampShader->use();
    lampShader->setMat4("projection", projection);
    lampShader->setMat4("view", view);

    float radius = 10.0f;
    float camX = sin(0.50f * time) * radius;
    float camZ = cos(0.50f * time) * radius;
    lightPos = glm::vec3(camX, 0, camZ);
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    /*
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
     */
    lampShader->setMat4("model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDisable(GL_DEPTH_TEST);
}

void LightSceneRenderer::buildBuffers(std::shared_ptr<OGLTexture> diffuse_map, std::shared_ptr<OGLTexture> specular_map) {
    mesh = std::unique_ptr<Mesh>(new Mesh(MeshUpdateType::STATIC));
    mesh->bindVAO();
    mesh->bindVBO();
    mesh->generateTexturedCubeWithNormals();
    mesh->upload();
    setupBufferAttributes();

    specularMap = std::move(specular_map);
    diffuseMap = std::move(diffuse_map);
}

void LightSceneRenderer::setupBufferAttributes() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("setup OpenGL error: %d", err);
    }
    // bind index 0 to position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // bind index 1 to normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // bind index 2 to texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // setup light vao
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // bind mesh again
    mesh->bindVBO();
    // bind index 0 to position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


