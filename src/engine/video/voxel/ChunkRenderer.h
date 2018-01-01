//
// Created by bison on 25-12-2017.
//

#ifndef GAME_TILERENDERER_H
#define GAME_TILERENDERER_H

#include <memory>
#include <engine/ISystem.h>
#include <array>
#include <engine/video/Shader.h>
#include <engine/video/Mesh3.h>
#include <engine/video/Camera.h>
#include <engine/video/MaterialDictionary.h>
#include <engine/asset/ILoadTask.h>
#include <engine/video/frustrum/ViewFrustum.h>
#include <map>
#include <unordered_map>
#include <engine/video/Fog.h>
#include <engine/video/Skybox.h>
#include "Chunk.h"
#include "BlockTypeDictionary.h"
#include "Terrain.h"
#include "ChunkPos.h"


class ChunkRenderer : public ILoadTask {
public:
    const int VISIBLE_RADIUS = 24;
    const int CHUNKS_BUILD_PER_FRAME = 2;

    ChunkRenderer(ISystem &system, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain);
    void render(float screenWidth, float screenHeight, double time);

    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void update();
    int32_t getActiveChunks();
    int32_t getRenderedChunks();
    ChunkPos camChunkPos;
    glm::vec3 worldPos;

private:
    ISystem& system;
    std::shared_ptr<Terrain> terrain = nullptr;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    std::unique_ptr<BlockTypeDictionary> blockTypeDict;
    std::map<ChunkPos, std::shared_ptr<Chunk>> activeChunks;
    std::map<ChunkPos, std::shared_ptr<Chunk>> buildChunks;
    std::vector<Chunk*> renderList;
    std::vector<std::shared_ptr<Chunk>> recycleList;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};
    std::unique_ptr<ViewFrustum> viewFrustrum;
    std::unique_ptr<Fog> fog;
    std::unique_ptr<Skybox> skybox;
    int32_t renderedChunks = 0;

    void worldToChunk(glm::vec3 &worldpos, ChunkPos &chunkpos);
    void chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos);
    Chunk* findChunkAt(const std::map<ChunkPos, std::shared_ptr<Chunk>>&chunk_map, const ChunkPos& pos);
    bool posInVisibleRadius(ChunkPos &pos);
    void runIncrementalChunkBuild();
};


#endif //GAME_TILERENDERER_H
