//
// Created by bison on 25-12-2017.
//

#include "ChunkRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>
#include <cmath>
#include <engine/video/shader/shaders.h>

ChunkRenderer::ChunkRenderer(IGame &game, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain) : game(game), camera(camera) {
    viewFrustrum = std::unique_ptr<ViewFrustum>(new ViewFrustum());
    chunkManager = std::unique_ptr<ChunkManager>(new ChunkManager(terrain));

    auto& system = *game.getSystem();
    auto lighting_vs = system.readTextFile("shaders/voxel_shader3_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/voxel_shader3_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    skybox = std::unique_ptr<Skybox>(new Skybox(system, camera));

    blockTypeDict = std::unique_ptr<BlockTypeDictionary>(new BlockTypeDictionary());
    blockTypeDict->createBlockType("stone", "assets/stone_diffuse.png", "assets/stone_specular.png", 32);
    blockTypeDict->createBlockType("grass", "assets/grass_diffuse.png", "assets/grass_specular.png", 8);
    blockTypeDict->createBlockType("water", "assets/water_diffuse.png", "assets/water_specular.png", 64);

    blockTypeDict->createBlockType("emerald",
                                 glm::vec4(0.0215f, 0.1745f, 0.0215f, 1.0f),
                                 glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f),
                                 glm::vec4(0.633f, 0.727811f, 0.633f, 1.0f),
                                 0.6f*128.0f);

    blockTypeDict->createBlockType("gold",
                                 glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f),
                                 glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f),
                                 glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f),
                                 0.4f*128.0f);

    blockTypeDict->createBlockType("silver",
                                 glm::vec4(0.19225f, 0.19225f, 0.19225f, 1.0f),
                                 glm::vec4(0.50754f, 0.50754f, 0.50754f, 1.0f),
                                 glm::vec4(0.508273f, 0.508273f, 0.508273f, 1.0f),
                                 0.4f*128.0f);

    blockTypeDict->createBlockType("bronze",
                                 glm::vec4(0.2125f, 0.1275f, 0.054f, 1.0f),
                                 glm::vec4(0.714f, 0.4284f, 0.18144f, 1.0f),
                                 glm::vec4(0.393548f, 0.271906f, 0.166721f, 1.0f),
                                 0.2f*128.0f);

    blockTypeDict->createBlockType("debug",
                                   glm::vec4(0.2125f, 0.1275f, 0.054f, 1.0f),
                                   glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                                   glm::vec4(0.393548f, 0.271906f, 0.166721f, 1.0f),
                                   0.2f*128.0f);

    //glm::vec3 color = glm::vec3(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f);
    glm::vec3 color = glm::vec3(0.3294f, 0.92157f, 1.0f);
    //fog = std::unique_ptr<Fog>(new Fog(color, 0.0075f, true));
    fog = std::unique_ptr<Fog>(new Fog(color, 0.0025f, true));
    directionalLight = std::unique_ptr<DirectionalLight>(new DirectionalLight());
}

void ChunkRenderer::render(float screenWidth, float screenHeight, double delta) {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);     // Cull back facing polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 600.0f);
    glm::mat4 view = camera->GetViewMatrix();
    viewFrustrum->setCamInternals(camera->Zoom, screenWidth / screenHeight, 0.1f, 600.0f);

    // update directional light
    updateDirectionalLight((float) delta);

    skybox->render(view, projection, directionalLight->intensity);

    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    sendNormalsArrayUniform();
    sendTexcoordArrayUniform();

    shader->setVec3("camPos", camera->Position);
    fog->color = directionalLight->intensity * glm::vec3(0.3294f, 0.92157f, 1.0f);
    fog->applyShader(*shader);

    // ambient light
    shader->setVec3("ambientLight", 0.15f, 0.15f, 0.15f);

    // directional light

    directionalLight->applyShader(*shader);


    Vec3 cam_pos(camera->Position.x, camera->Position.y, camera->Position.z);
    Vec3 cam_front(camera->Front.x, camera->Front.y, camera->Front.z);
    Vec3 cam_up(camera->Up.x, camera->Up.y, camera->Up.z);
    Vec3 cam_dir = cam_pos + cam_front;
    viewFrustrum->setCamDef(cam_pos, cam_dir, cam_up);

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    renderList.clear();
    AABox chunkbox;
    Vec3 corner;
    for(auto& chunk : chunkManager->activeChunks) {
        corner.x = chunk.second->position.x;
        corner.y = chunk.second->position.y;
        corner.z = chunk.second->position.z;
        //corner.x -= halfChunkSize;
        //corner.z -= halfChunkSize;
        chunkbox.setBox(corner, CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE);
        //SDL_Log("Chunk pos %.2f,%.2f,%.2f", chunk.second->position.x, chunk.second->position.y, chunk.second->position.z);
        //SDL_Log("Box Corner pos %.2f,%.2f,%.2f, xyz = %.2f,%.2f,%.2f", chunkbox.corner.x, chunkbox.corner.y, chunkbox.corner.z, chunkbox.x, chunkbox.y, chunkbox.z);
        if(viewFrustrum->boxInFrustum(chunkbox) != ViewFrustum::OUTSIDE )
            renderList.push_back(chunk.second.get());
    }

    renderedChunks = renderList.size();
    for(auto chunk : renderList)
    {
        glm::mat4 model_m;
        model_m = glm::translate(model_m, chunk->position);
        shader->setMat4("model", model_m);
        chunk->draw(*shader);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

bool ChunkRenderer::load(IGame &game) {
    blockTypeDict->load(game);
    return true;
}

bool ChunkRenderer::prepare(IGame &game) {
    blockTypeDict->prepare(game);
    skybox->prepare();
    return true;
}

static int distance(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}


void ChunkRenderer::update() {
  chunkManager->update(camera->Position, *blockTypeDict);
}

i32 ChunkRenderer::getActiveChunks() {
    return static_cast<i32>(chunkManager->activeChunks.size());
}

i32 ChunkRenderer::getRenderedChunks() {
    return renderedChunks;
}

void ChunkRenderer::updateDirectionalLight(float delta) {
// Update directional light direction, intensity and colour
    float fast = 5.0f;
    float slow = 2.0f;
    if (lightAngle > 90) {
        //lightAngle += fast * delta;
        directionalLight->intensity = 0;
        if (lightAngle >= 360) {
            lightAngle = -90;
        }
    } else if (lightAngle <= -80 || lightAngle >= 80) {
        //lightAngle += slow * delta;
        float factor = 1 - (abs(lightAngle) - 80)/ 10.0f;
        directionalLight->intensity = factor;
        directionalLight->color.y = std::max(factor, 0.9f);
        directionalLight->color.z = std::max(factor, 0.5f);
    } else {
        //lightAngle += fast * delta;
        directionalLight->intensity = 1.0f;
        directionalLight->color.x = 1.0f;
        directionalLight->color.y = 1.0f;
        directionalLight->color.z = 1.0f;
    }
    double angRad = glm::radians(lightAngle);
    directionalLight->direction.x = (float) sin(angRad);
    directionalLight->direction.y = -1.0f * (float) cos(angRad);
    //directionalLight->intensity = 1.0f;
    //SDL_Log("Light angle %.2f intensity %.2f", lightAngle, directionalLight->intensity);
    if(lightAngle > 100)
        lightAngle = -90;
}

// TODO use uniform buffer objects instead
void ChunkRenderer::sendNormalsArrayUniform()
{
    // Back face
    shader->setVec3("normals[0]",0,0,-1);
    // Front face
    shader->setVec3("normals[1]",0,0,1);
    // Left face
    shader->setVec3("normals[2]",-1,0,0);
    // Right face
    shader->setVec3("normals[3]",1,0,0);
    // Bottom face
    shader->setVec3("normals[4]",0,-1,0);
    // Top face
    shader->setVec3("normals[5]",0,1,0);
}

// TODO use uniform buffer objects instead
void ChunkRenderer::sendTexcoordArrayUniform()
{
    shader->setVec2("texcoords[0]",0,0);
    shader->setVec2("texcoords[1]",0,1);
    shader->setVec2("texcoords[2]",1,0);
    shader->setVec2("texcoords[3]",1,1);
}