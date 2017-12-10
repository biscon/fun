//
// Created by bison on 21-11-2017.
//

#ifndef GAME_CUBERENDERER_H
#define GAME_CUBERENDERER_H

#include "OGLTexture.h"
#include "Shader.h"
#include "Mesh.h"

class CubeRenderer {
public:
    explicit CubeRenderer(std::shared_ptr<TextureAtlas> textureAtlas);


    void render(float screenWidth, float screenHeight);
    void startFrame();
    void buildBuffers();

    uint32_t getVertexCount() const {
        return vertexCount/7;
    }

private:
    void setupBufferAttributes();
    std::unique_ptr<Shader> shader = nullptr;
    std::unique_ptr<Mesh> mesh = nullptr;
    std::shared_ptr<TextureAtlas> textureAtlas;

    uint32_t vertexCount = 0;
};


#endif
