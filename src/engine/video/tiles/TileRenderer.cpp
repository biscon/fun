//
// Created by bison on 25-12-2017.
//

#include "TileRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

TileRenderer::TileRenderer(ISystem &system, const std::shared_ptr<Camera> &camera) : system(system), camera(camera) {
    auto lighting_vs = system.readTextFile("shaders/lit_tile_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/lit_tile_shader_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");

    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    auto lamp_vs = system.readTextFile("shaders/lamp_shader_vs.glsl");
    auto lamp_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    lampShader = std::unique_ptr<Shader>(new Shader(lamp_vs.c_str(), lamp_fs.c_str(), nullptr));

    /*
    materialDict = std::unique_ptr<MaterialDictionary>(new MaterialDictionary());
    materialDict->createMaterial("emerald",
                                 glm::vec3(0.0215f, 0.1745f, 0.0215f),
                                 glm::vec3(0.07568f, 0.61424f, 0.07568f),
                                 glm::vec3(0.633f, 0.727811f, 0.633f),
                                 "", "", 0.6f*128.0f);

    materialDict->createMaterial("gold",
                                 glm::vec3(0.24725f, 0.1995f, 0.0745f),
                                 glm::vec3(0.75164f, 0.60648f, 0.22648f),
                                 glm::vec3(0.628281f, 0.555802f, 0.366065f),
                                 "", "", 0.4f*128.0f);

    materialDict->createMaterial("silver",
                                 glm::vec3(0.19225f, 0.19225f, 0.19225f),
                                 glm::vec3(0.50754f, 0.50754f, 0.50754f),
                                 glm::vec3(0.508273f, 0.508273f, 0.508273f),
                                 "", "", 0.4f*128.0f);

    materialDict->createMaterial("bronze",
                                 glm::vec3(0.2125f, 0.1275f, 0.054f),
                                 glm::vec3(0.714f, 0.4284f, 0.18144f),
                                 glm::vec3(0.393548f, 0.271906f, 0.166721f),
                                 "", "", 0.2f*128.0f);
    */

    tileTypeDict = std::unique_ptr<TileTypeDictionary>(new TileTypeDictionary());
    tileTypeDict->createTileType("grass", "assets/grass_diffuse.png", "assets/grass_specular.png", 32);
    tileTypeDict->createTileType("wood", "assets/wood_diffuse.png", "assets/wood_specular.png", 32);
    tileTypeDict->createTileType("stone", "assets/stone_diffuse.png", "assets/stone_specular.png", 32);
    tileTypeDict->createTileType("wood2", "assets/wood_diffuse.png", "assets/wood_specular.png", 32);
}

void TileRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);
    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    shader->setVec3("viewPos", camera->Position);

    // directional light
    shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setVec3("dirLight.ambient", 0.025f, 0.025f, 0.025f);
    shader->setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.10f);
    shader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);
    /*
    shader->setVec3("dirLight.diffuse", 0.25f, 0.25f, 0.25f);
    shader->setVec3("dirLight.specular", 0.30f, 0.30f, 0.30f);
    */
    /*
    shader->setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
    shader->setVec3("dirLight.specular", 0.4f, 0.4f, 0.4f);
    */
    /*
    shader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    */

    float radius = 4.0f;
    float camX = sin(0.50f * time) * radius;
    float camZ = cos(0.50f * time) * radius;
    lightPos = glm::vec3(camX, 2.0f, camZ);
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

    // draw model

    for(auto& chunk : visibleList) {
        glm::mat4 model_m;
        model_m = glm::translate(model_m, chunk->position); // translate it down so it's at the center of the scene
        shader->setMat4("model", model_m);
        chunk->draw(*shader);
    }

    /*
    lampShader->use();
    lampShader->setMat4("projection", projection);
    lampShader->setMat4("view", view);
    model_m = glm::mat4();
    model_m = glm::translate(model_m, lightPos);
    model_m = glm::scale(model_m, glm::vec3(0.2f)); // a smaller cube
    lampShader->setMat4("model", model_m);
    model->draw(*lampShader);
    */
    glDisable(GL_DEPTH_TEST);
}

bool TileRenderer::load(IGame &game) {
    tileTypeDict->load(game);
    return true;
}

bool TileRenderer::prepare(IGame &game) {
    tileTypeDict->prepare(game);
    chunk = std::unique_ptr<TileChunk>(new TileChunk(*tileTypeDict));
    chunk->rebuild();
    return true;
}

void TileRenderer::buildVisibleList()
{
    visibleList.clear();
    // calculate width of a box centered on the camera pos
    int size = (VISIBLE_RADIUS * 2) + 1;
    float fsize = (float) size;
    auto m = glm::mat4();
    auto origin = glm::vec4(0,0,0,1);
    float cs = (float) TileChunk::CHUNK_SIZE;
    m = glm::translate(m, glm::vec3((-0.5 * (fsize * cs)) - cs/2, -0.5f, (-0.5 * (fsize * cs)) + cs/2));
    for(int y = 0; y < size; y++)
    {
        glm::mat4 old = m;
        for(int x = 0; x < size; x++)
        {
            m = glm::translate(m, glm::vec3(cs, 0.0f, 0.0f));
            auto pos = m * origin;
            //SDL_Log("Build chunk at %.2f,%.2f,%.2f", pos.x, pos.y, pos.z);
            visibleList.push_back(std::make_shared<TileChunk>(*tileTypeDict));
            visibleList.back()->position = pos;
            visibleList.back()->rebuild();
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, cs));
    }
    SDL_Log("Build visible list of %d chunks", visibleList.size());
}

void TileRenderer::update() {
    if(visibleList.empty())
    {
        buildVisibleList();
    }
}
