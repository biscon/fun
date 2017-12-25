//
// Created by bison on 25-12-2017.
//

#include "ModelRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

ModelRenderer::ModelRenderer(ISystem &system, const std::shared_ptr<Model> &model,
                             const std::shared_ptr<Camera> &camera) : system(system), model(model), camera(camera) {
    auto lighting_vs = system.readTextFile("shaders/material_lighting_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/material_lighting_shader_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");

    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    auto lamp_vs = system.readTextFile("shaders/lamp_shader_vs.glsl");
    auto lamp_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    lampShader = std::unique_ptr<Shader>(new Shader(lamp_vs.c_str(), lamp_fs.c_str(), nullptr));

}

void ModelRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);
    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    shader->setVec3("viewPos", camera->Position);

    /*
    shader->setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    shader->setVec3("light.diffuse",  0.8f, 0.8f, 0.8f); // darken the light a bit to fit the scene
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader->setVec3("light.position", lightPos);
     */


    // directional light
    shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    /*
    shader->setVec3("dirLight.diffuse", 0.25f, 0.25f, 0.25f);
    shader->setVec3("dirLight.specular", 0.30f, 0.30f, 0.30f);
    */
    shader->setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
    shader->setVec3("dirLight.specular", 0.4f, 0.4f, 0.4f);

    float radius = 4.0f;
    float camX = sin(0.50f * time) * radius;
    float camZ = cos(0.50f * time) * radius;
    lightPos = glm::vec3(camX, 2.0f, camZ-7.0f);
    //lightPos[1] = glm::vec3(sin(0.50f * time) * 10, cos(0.50f * time) * 6, -7.0f);
    // point light 1
    shader->setVec3("pointLights[0].position", lightPos);
    shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("pointLights[0].constant", 1.0f);
    //shader->setFloat("pointLights[0].linear", 0.022f);
    //shader->setFloat("pointLights[0].quadratic", 0.0019f);
    shader->setFloat("pointLights[0].linear", 0.09);
    shader->setFloat("pointLights[0].quadratic", 0.032);
    // spotLight
    shader->setVec3("spotLight.position", camera->Position);
    shader->setVec3("spotLight.direction", camera->Front);
    shader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLight.constant", 1.0f);
    shader->setFloat("spotLight.linear", 0.09);
    shader->setFloat("spotLight.quadratic", 0.032);
    shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // world transformation
/*
    glm::mat4 model;
    model = glm::rotate(model, (float)(time) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    shader->setMat4("model", model);
*/

    // draw model
    glm::mat4 model_m;
    model_m = glm::translate(model_m, glm::vec3(0.0f, -0.0f, -7.0f)); // translate it down so it's at the center of the scene
    //model_m = glm::scale(model_m, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
    shader->setMat4("model", model_m);
    model->draw(*shader);


    lampShader->use();
    lampShader->setMat4("projection", projection);
    lampShader->setMat4("view", view);
    model_m = glm::mat4();
    model_m = glm::translate(model_m, lightPos);
    model_m = glm::scale(model_m, glm::vec3(0.2f)); // a smaller cube
    lampShader->setMat4("model", model_m);
    model->draw(*lampShader);

    glDisable(GL_DEPTH_TEST);
}
