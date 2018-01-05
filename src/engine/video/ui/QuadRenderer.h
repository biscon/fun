//
// Created by bison on 21-11-2017.
//

#ifndef GAME_QUADRENDERER_H
#define GAME_QUADRENDERER_H

#include "engine/video/mesh/IndexedMesh.h"
#include "engine/video/texture/OGLTexture.h"
#include "engine/video/shader/Shader.h"

class QuadRenderer {
public:
    explicit QuadRenderer(std::shared_ptr<TextureAtlas> textureAtlas);

    void drawTexturedQuad(i32 handle, float left, float top, float right, float bottom, i32 layer);
    void render(float screenWidth, float screenHeight);
    void startFrame();
    void buildBuffers();

    u32 getVertexCount() const {
        return vertexCount/7;
    }

private:
    void setupBufferAttributes();
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<IndexedMesh> mesh = nullptr;
    std::shared_ptr<TextureAtlas> textureAtlas;

    u32 vertexCount = 0;
};


#endif //GAME_QUADRENDERER_H
