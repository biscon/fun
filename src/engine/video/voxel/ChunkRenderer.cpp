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
#include <engine/video/shaders.h>

ChunkRenderer::ChunkRenderer(ISystem &system, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain) : system(system), camera(camera), terrain(terrain) {
    viewFrustrum = std::unique_ptr<ViewFrustum>(new ViewFrustum());

    auto lighting_vs = system.readTextFile("shaders/lit_tile_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/lit_tile_shader_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");

    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    auto lamp_vs = system.readTextFile("shaders/lamp_shader_vs.glsl");
    auto lamp_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    lampShader = std::unique_ptr<Shader>(new Shader(lamp_vs.c_str(), lamp_fs.c_str(), nullptr));

    skybox = std::unique_ptr<Skybox>(new Skybox(system, camera));

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

    blockTypeDict = std::unique_ptr<BlockTypeDictionary>(new BlockTypeDictionary());
    blockTypeDict->createBlockType("stone", "assets/stone_diffuse.png", "assets/stone_specular.png", 32);
    blockTypeDict->createBlockType("grass", "assets/grass_diffuse.png", "assets/grass_specular.png", 4);
    blockTypeDict->createBlockType("water", "assets/water_diffuse.png", "assets/water_specular.png", 64);

    //glm::vec3 color = glm::vec3(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f);
    glm::vec3 color = glm::vec3(0.3294f, 0.92157f, 1.0f);
    //fog = std::unique_ptr<Fog>(new Fog(color, 0.0075f, true));
    fog = std::unique_ptr<Fog>(new Fog(color, 0.0125f, true));
}

void ChunkRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);     // Cull back facing polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), screenWidth / screenHeight, 0.1f, 800.0f);
    glm::mat4 view = camera->GetViewMatrix();
    viewFrustrum->setCamInternals(camera->Zoom, screenWidth / screenHeight, 0.1f, 800.0f);

    skybox->render(view, projection);

    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    shader->setVec3("viewPos", camera->Position);
    fog->applyShader(*shader);

    // directional light
    //shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader->setVec3("dirLight.ambient", 0.65f, 0.65f, 0.65f);

    /*
    shader->setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.10f);
    shader->setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
     */
    /*
    shader->setVec3("dirLight.diffuse", 0.25f, 0.25f, 0.25f);
    shader->setVec3("dirLight.specular", 0.30f, 0.30f, 0.30f);
    */

    /*
    shader->setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
    shader->setVec3("dirLight.specular", 0.4f, 0.4f, 0.4f);
    */

    shader->setVec3("dirLight.diffuse", 0.95f, 0.95f, 0.95f);
    shader->setVec3("dirLight.specular", 0.15f, 0.15f, 0.15f);


    float radius = 4.0f;
    float camX = sin(0.50f * time) * radius;
    float camZ = cos(0.50f * time) * radius;
    lightPos = glm::vec3(camX, 34.0f, camZ);
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


    Vec3 cam_pos(camera->Position.x, camera->Position.y, camera->Position.z);
    Vec3 cam_front(camera->Front.x, camera->Front.y, camera->Front.z);
    Vec3 cam_up(camera->Up.x, camera->Up.y, camera->Up.z);
    Vec3 cam_dir = cam_pos + cam_front;
    viewFrustrum->setCamDef(cam_pos, cam_dir, cam_up);

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    renderList.clear();
    AABox chunkbox;
    float half_chunk_size = (float) Chunk::CHUNK_SIZE/2;
    float half_chunk_height = (float) Chunk::CHUNK_HEIGHT/2;
    Vec3 corner;
    for(auto& chunk : activeChunks) {
        corner.x = chunk.second->position.x;
        corner.y = chunk.second->position.y;
        corner.z = chunk.second->position.z;


        corner.x -= half_chunk_size;
        //corner.y -= half_chunk_height;
        corner.z -= half_chunk_size;

        chunkbox.setBox(corner, Chunk::CHUNK_SIZE, Chunk::CHUNK_HEIGHT, Chunk::CHUNK_SIZE);
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

bool ChunkRenderer::posInVisibleRadius(ChunkPos& pos)
{
    return distance(camChunkPos.x, camChunkPos.z, pos.x, pos.z) <= VISIBLE_RADIUS;
}

void ChunkRenderer::runIncrementalChunkBuild()
{
    int32_t count = 0;
    while(!buildChunks.empty() && count < CHUNKS_BUILD_PER_FRAME)
    {
        auto chunk_it = buildChunks.begin();
        chunk_it->second->rebuild(chunk_it->first, terrain);
        activeChunks[chunk_it->first] = chunk_it->second;
        buildChunks.erase(chunk_it);
        count++;
    }
    //SDL_Log("Rebuild %d chunks this frame", count);
}

void ChunkRenderer::update() {
    worldToChunk(camera->Position, camChunkPos);
    chunkToWorld(camChunkPos, worldPos);

    bool did_rebuild = false;
    // check if chunks needs to be added, by checking a grid based on the visual radius around the camera
    // calculate width of a box centered on the camera pos
    int size = (VISIBLE_RADIUS * 2) + 1;
    auto m = glm::mat4();

    int chunks_allocated = 0;
    int chunks_recycled = 0;
    int radius = VISIBLE_RADIUS;
    for(int z=-radius; z<=radius; z++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + z * z <= radius * radius)
            {
                ChunkPos testpos;
                glm::vec3 worldpos = {0, 0.5f, 0};
                testpos.x = camChunkPos.x + x;
                testpos.z = camChunkPos.z + z;
                chunkToWorld(testpos, worldpos);
                auto chunk = findChunkAt(activeChunks, testpos);
                auto build_chunk = findChunkAt(buildChunks, testpos);
                if(chunk == nullptr && build_chunk == nullptr)
                {
                    did_rebuild = true;
                    //SDL_Log("No chunk found at pos %d,%d, adding one.", testpos.x, testpos.z);
                    if(!recycleList.empty())
                    {
                        chunks_recycled++;
                        std::shared_ptr<Chunk> nc = recycleList.back();
                        recycleList.pop_back();
                        nc->position = worldpos;
                        buildChunks[testpos] = nc;
                    }
                    else {
                        chunks_allocated++;
                        auto nc = std::make_shared<Chunk>(*blockTypeDict);
                        nc->position = worldpos;
                        buildChunks[testpos] = nc;
                    }
                }
            }
        }
    }

    if(!buildChunks.empty())
    {
        auto start = system.getPerformanceCounter();
        runIncrementalChunkBuild();
        auto end = system.getPerformanceCounter();
        auto diff = end - start;
        //SDL_Log("Incremental chunk building took %d ms this frame (%d chunks built)", diff / (system.getPerformanceFreq()/1000), CHUNKS_BUILD_PER_FRAME);
    }

    if(did_rebuild)
        SDL_Log("Started rebuilding %d chunks. new = %d, recycled = %d", chunks_recycled+chunks_allocated, chunks_allocated, chunks_recycled);

    // check if chunks need to be removed
    for (auto it = activeChunks.cbegin(); it != activeChunks.cend() /* not hoisted */; /* no increment */)
    {
        auto tilepos = (*it).first;
        //auto dist = distance(camChunkPos.x, camChunkPos.z, tilepos.x, tilepos.z);
        auto visible = posInVisibleRadius(tilepos);
        if (!visible)
        {
            auto chunk = (*it).second;
            recycleList.push_back(chunk);
            activeChunks.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}

void ChunkRenderer::worldToChunk(glm::vec3& worldpos, ChunkPos& chunkpos)
{
    auto xoffset = (float) Chunk::CHUNK_SIZE/2;
    auto zoffset = (float) Chunk::CHUNK_SIZE/2;
    if(worldpos.x < 0)
        xoffset *= -1;
    if(worldpos.z < 0)
        zoffset *= -1;
    chunkpos.x = (int) ((worldpos.x + xoffset) / (float) Chunk::CHUNK_SIZE);
    chunkpos.z = (int) ((worldpos.z + zoffset) / (float) Chunk::CHUNK_SIZE);
}

void ChunkRenderer::chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos) {
    worldpos.x = chunkpos.x * Chunk::CHUNK_SIZE;
    worldpos.z = chunkpos.z * Chunk::CHUNK_SIZE;
}

Chunk *ChunkRenderer::findChunkAt(const std::map<ChunkPos, std::shared_ptr<Chunk>>&chunk_map, const ChunkPos &pos) {
    auto it = chunk_map.find(pos);
    if(it != chunk_map.end())
        return (*it).second.get();

    return nullptr;
}

int32_t ChunkRenderer::getActiveChunks() {
    return static_cast<int32_t>(activeChunks.size());
}

int32_t ChunkRenderer::getRenderedChunks() {
    return renderedChunks;
}
