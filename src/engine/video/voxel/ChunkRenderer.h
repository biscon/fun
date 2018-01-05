//
// Created by bison on 25-12-2017.
//

#ifndef GAME_TILERENDERER_H
#define GAME_TILERENDERER_H

#include <memory>
#include <engine/system/ISystem.h>
#include <array>
#include <engine/video/shader/Shader.h>
#include <engine/video/mesh/Mesh3.h>
#include <engine/video/camera/Camera.h>
#include <engine/video/model/MaterialDictionary.h>
#include <engine/asset/ILoadTask.h>
#include <engine/video/frustrum/ViewFrustum.h>
#include <map>
#include <unordered_map>
#include <engine/video/shader/Fog.h>
#include <engine/video/sky/Skybox.h>
#include <engine/video/shader/DirectionalLight.h>
#include "Chunk.h"
#include "BlockTypeDictionary.h"
#include "Terrain.h"
#include "ChunkPos.h"

#define CHUNK_STAGE_SETUP 0
#define CHUNK_STAGE_BUILD 1

// TODO refactor parts into a chunkmap object to pass to individual chunks and to break up this big class, allow chunks to query their neighbours

// Do not copy shared_ptr's in inner loops (they incur reference and synchronization overhead)
// unique_ptr's on the other hand should optimize to bare pointers
class ChunkRenderer : public ILoadTask {
public:
    const int VISIBLE_RADIUS = 4;
    const int CHUNKS_SETUP_PER_FRAME = 4;
    const int CHUNKS_BUILD_PER_FRAME = 4;

    ChunkRenderer(ISystem &system, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain);
    void render(float screenWidth, float screenHeight, double time);

    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void update();
    i32 getActiveChunks();
    i32 getRenderedChunks();
    ChunkPos camChunkPos;
    glm::vec3 worldPos;

private:
    ISystem& system;
    std::shared_ptr<Terrain> terrain = nullptr;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    std::unique_ptr<BlockTypeDictionary> blockTypeDict;
    std::map<ChunkPos, std::unique_ptr<Chunk>> activeChunks;
    std::map<ChunkPos, std::unique_ptr<Chunk>> buildChunks;
    std::vector<Chunk*> renderList;
    std::vector<std::unique_ptr<Chunk>> recycleList;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};
    std::unique_ptr<ViewFrustum> viewFrustrum;
    std::unique_ptr<Fog> fog;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<DirectionalLight> directionalLight;
    float lightAngle = -90.0f;
    i32 renderedChunks = 0;

    i32 buildStage = 0;
    std::map<ChunkPos, std::unique_ptr<Chunk>>::iterator setupIterator;

    void worldToChunk(glm::vec3 &worldpos, ChunkPos &chunkpos);
    void chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos);
    //Chunk* findChunkAt(const std::map<ChunkPos, std::unique_ptr<Chunk>>& chunk_map, const ChunkPos& pos);
    //bool posInVisibleRadius(ChunkPos &pos);
    void runIncrementalChunkBuild();
    void updateDirectionalLight();

    inline bool posInVisibleRadius(ChunkPos& pos)
    {
        int dx = pos.x - camChunkPos.x;
        int dy = pos.z - camChunkPos.z;
        return sqrt(dx*dx + dy*dy) <= VISIBLE_RADIUS;
    }


    inline i32 posToIndex(i32 x, i32 z)
    {
        return z * CHUNK_SIZE + x;
    }

    inline Chunk *findChunkAt(const std::map<ChunkPos, std::unique_ptr<Chunk>>&chunk_map, const ChunkPos &pos) {
        auto it = chunk_map.find(pos);
        if(it != chunk_map.end())
            return (*it).second.get();

        return nullptr;
    }

};


#endif //GAME_TILERENDERER_H
