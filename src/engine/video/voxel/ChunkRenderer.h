//
// Created by bison on 25-12-2017.
//

#ifndef GAME_TILERENDERER_H
#define GAME_TILERENDERER_H

#include <memory>
#include <engine/system/ISystem.h>
#include <array>
#include <engine/video/shader/Shader.h>
#include <engine/video/mesh/BlockMesh.h>
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
#include "ChunkManager.h"

// Do not copy shared_ptr's in inner loops (they incur reference and synchronization overhead)
// unique_ptr's on the other hand should optimize to bare pointers (just avoid syphilis_ptr completely)
class ChunkRenderer : public ILoadTask {
public:
    ChunkRenderer(IGame &game, const std::shared_ptr<Camera> &camera, const std::shared_ptr<Terrain> &terrain);
    void render(float screenWidth, float screenHeight, double time);

    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void update();
    i32 getActiveChunks();
    i32 getRenderedChunks();
    //ChunkPos camChunkPos;
    std::unique_ptr<ChunkManager> chunkManager;

private:
    IGame& game;

    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<BlockTypeDictionary> blockTypeDict;
    std::vector<Chunk*> renderList;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};
    std::unique_ptr<ViewFrustum> viewFrustrum;
    std::unique_ptr<Fog> fog;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<DirectionalLight> directionalLight;
    float lightAngle = -90.0f;
    i32 renderedChunks = 0;

    void updateDirectionalLight(float delta);

    inline i32 posToIndex(i32 x, i32 z)
    {
        return z * CHUNK_SIZE + x;
    }

    void sendNormalsArrayUniform();

    void sendTexcoordArrayUniform();
};


#endif //GAME_TILERENDERER_H
