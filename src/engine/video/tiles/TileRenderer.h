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
#include <map>
#include <unordered_map>
#include "TileChunk.h"
#include "TileTypeDictionary.h"

struct ChunkPos
{
    int32_t x;
    int32_t z;
    /*
    bool operator<(const ChunkPos r)
    {
        if ( l.x < r.x )  return true;
        if ( l.x > r.x )  return false;

        // Otherwise a are equal
        if ( l.z < r.z )  return true;
        if ( l.z > r.z )  return false;
        // Otherwise both are equal
        return false;
    }
    */

    bool operator<(const ChunkPos &rhs) const {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return z < rhs.z;
    }

    /*
    bool operator>(const ChunkPos &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const ChunkPos &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const ChunkPos &rhs) const {
        return !(*this < rhs);
    }
     */
};

class TileRenderer : public ILoadTask {
public:
    const int VISIBLE_RADIUS = 4;

    TileRenderer(ISystem &system, const std::shared_ptr<Camera> &camera);
    void render(float screenWidth, float screenHeight, double time);

    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void update();
    int32_t getActiveChunks();
    ChunkPos camChunkPos;
    glm::vec3 worldPos;

private:
    ISystem& system;
    std::shared_ptr<Camera> camera = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Shader> lampShader = nullptr;
    std::unique_ptr<TileTypeDictionary> tileTypeDict;
    std::map<ChunkPos, std::shared_ptr<TileChunk>> activeChunks;
    std::vector<std::shared_ptr<TileChunk>> renderList;
    std::vector<std::shared_ptr<TileChunk>> recycleList;
    glm::vec3 lightPos = {0.0f, 2.0f, -6.0f};
    std::shared_ptr<TileChunk> chunk;

    void worldToChunk(glm::vec3 &worldpos, ChunkPos &chunkpos);
    void chunkToWorld(ChunkPos &chunkpos, glm::vec3 &worldpos);
    //TileChunk* findChunkAt(const std::vector<std::shared_ptr<TileChunk>>& tilelist, const ChunkPos& pos);
    TileChunk* findChunkAt(const ChunkPos& pos);

    bool posInVisibleRadius(ChunkPos &pos);
};


#endif //GAME_TILERENDERER_H
