//
// Created by bison on 21-11-2017.
//

#ifndef GAME_CUBERENDERER_H
#define GAME_CUBERENDERER_H

#include "engine/video/texture/OGLTexture.h"
#include "engine/video/shader/Shader.h"
#include "engine/video/mesh/Mesh.h"

class CubeRenderer {
public:
    explicit CubeRenderer(std::shared_ptr<TextureAtlas> textureAtlas);


    void render(float screenWidth, float screenHeight, double time);
    void buildBuffers();

    u32 getVertexCount() const {
        return vertexCount/7;
    }

private:
    void setupBufferAttributes();
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Mesh> mesh = nullptr;
    std::shared_ptr<TextureAtlas> textureAtlas;

    u32 vertexCount = 0;
};


#endif
