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
#include <engine/util/string_util.h>

ChunkRenderer::ChunkRenderer(IGame &game, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain) : game(game), camera(camera) {
    viewFrustrum = std::unique_ptr<ViewFrustum>(new ViewFrustum());

    auto& system = *game.getSystem();
    auto lighting_vs = system.readTextFile("shaders/voxel_shader3_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/voxel_shader3_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));
    printShaderUniformLocations();

    skybox = std::unique_ptr<Skybox>(new Skybox(system, camera));

    blockTypeDict = std::unique_ptr<BlockTypeDictionary>(new BlockTypeDictionary());
    blockTypeDict->createBlockType("stone", "assets/stone.png");

    blockTypeDict->createBlockType("grass", "assets/grass_side.png");
    auto* grass = blockTypeDict->getBlockTypeByName("grass");
    //blockTypeDict->setFaceColor(grass, TOP_FACE, glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f));
    blockTypeDict->setFaceTexture(grass, TOP_FACE, "assets/grass_top.png");
    blockTypeDict->setFaceTexture(grass, BOTTOM_FACE, "assets/grass_top.png");

    blockTypeDict->createBlockType("water", "assets/water_diffuse.png");

    blockTypeDict->createBlockType("emerald", glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f));

    blockTypeDict->createBlockType("gold", glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f));

    blockTypeDict->createBlockType("silver", glm::vec4(0.50754f, 0.50754f, 0.50754f, 1.0f));

    blockTypeDict->createBlockType("bronze", glm::vec4(0.714f, 0.4284f, 0.18144f, 1.0f));

    blockTypeDict->createBlockType("debug", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    blockTypeDict->createBlockType("planks", "assets/planks_acacia.png");
    blockTypeDict->createBlockType("glass", "assets/glass.png");
    blockTypeDict->createBlockType("brick", "assets/brick.png");

    blockTypeDict->createBlockType("sand", "assets/sand.png");
    blockTypeDict->createBlockType("red_sand", "assets/red_sand.png");

    //glm::vec3 color = glm::vec3(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f);
    glm::vec3 color = glm::vec3(0.3294f, 0.92157f, 1.0f);
    //fog = std::unique_ptr<Fog>(new Fog(color, 0.0075f, true));
    fog = std::unique_ptr<Fog>(new Fog(color, 0.0025f, true));
    directionalLight = std::unique_ptr<DirectionalLight>(new DirectionalLight());

    chunkManager = std::unique_ptr<AChunkManager>(new AChunkManager(game, *blockTypeDict, terrain));
}

void ChunkRenderer::printShaderUniformLocations()
{

    i32 layers = shader->getUniformLocation("layers");
    i32 colors = shader->getUniformLocation("colors");

    //SDL_Log("layers off = %d", layers);
    //SDL_Log("colors off = %d", colors);
    BlockType::layersUniformLocation = layers;
    BlockType::colorsUniformLocation = colors;
}

// TODO
/*
    Draw all opaque objects first.
    Sort all the transparent objects.
    Draw all the transparent objects in sorted order.
 */
void ChunkRenderer::render(float screenWidth, float screenHeight, double delta) {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);     // Cull back facing polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 1000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    viewFrustrum->setCamInternals(camera->Zoom, screenWidth / screenHeight, 0.1f, 1000.0f);

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
    //directionalLight->applyShader(*shader);

    // pass sunlight intensity to shader
    shader->setFloat("sunlightIntensity", directionalLight->intensity);


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

    // (farthest to nearest cuz transparent blocks)
    ChunkPos testpos;
    ChunkPos *positions = chunkManager->circlePosOffsets.data();
    for(i32 i = (i32) chunkManager->circlePosOffsets.size()-1; i-- > 0; ) {
        testpos.x = chunkManager->camChunkPos.x + positions[i].x;
        testpos.z = chunkManager->camChunkPos.z + positions[i].z;
        Chunk *chunk = chunkManager->findActiveChunkAt(testpos);
        if(chunk != nullptr) {
            corner.x = chunk->position.x;
            corner.y = chunk->position.y;
            corner.z = chunk->position.z;
            chunkbox.setBox(corner, CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE);
            if (viewFrustrum->boxInFrustum(chunkbox) != ViewFrustum::OUTSIDE)
                renderList.push_back(chunk);
        }
    }

    std::unique_lock<std::mutex> lock(chunkManager->activeLock);
    renderedChunks = renderList.size();
    blockTypeDict->arrayTexture->bind();
    shader->setInt("array_texture", 0);
    for(auto chunk : renderList)
    {
        glm::mat4 model_m;
        model_m = glm::translate(model_m, chunk->position);
        shader->setMat4("model", model_m);
        if(!chunk->isPrepared || chunk->needUpload)
            chunk->upload();
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
  chunkManager->update(camera->Position);
}

i32 ChunkRenderer::getActiveChunks() {
    return chunkManager->getActiveChunkCount();
}

i32 ChunkRenderer::getRenderedChunks() {
    return renderedChunks;
}

void ChunkRenderer::updateDirectionalLight(float delta) {
// Update directional light direction, intensity and colour
    float fast = 5.0f;
    float slow = 1.0f;
    if (lightAngle > 90) {
        lightAngle += 2 * fast * delta;
        directionalLight->intensity = 0;
        if (lightAngle >= 360) {
            lightAngle = -90;
        }
    } else if (lightAngle <= -80 || lightAngle >= 80) {
        lightAngle += slow * delta;
        float factor = 1 - (abs(lightAngle) - 80)/ 10.0f;
        directionalLight->intensity = factor;
        directionalLight->color.y = std::max(factor, 0.9f);
        directionalLight->color.z = std::max(factor, 0.5f);
    } else {
        lightAngle += fast * delta;
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

    /*
    if(lightAngle > 100)
        lightAngle = -90;
        */
    chunkManager->sunlightIntensity = directionalLight->intensity;
    //chunkManager->sunlightIntensity = std::max((-1.0f * directionalLight->direction.y) * 0.96f + 0.6f, 0.02f);
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