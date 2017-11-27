//
// Created by bison on 21-11-2017.
//

#ifndef GAME_QUADRENDERER_H
#define GAME_QUADRENDERER_H

#define QUAD_LAYERS 4


#include "Shader.h"
#include "RenderMesh.h"
#include "OGLTexture.h"
#include "TextureManager.h"

struct QuadLayer {
    std::map<TextureAtlas*, std::unique_ptr<RenderMesh>> meshMap;
};

class QuadRenderer {
public:
    explicit QuadRenderer(TextureManager& textureManager);

    void drawTexturedQuad(int32_t handle, float left, float top, float right, float bottom, int32_t layer);
    void render(float screenWidth, float screenHeight);
    void startFrame();
    void buildBuffers();

    uint32_t getVertexCount() const {
        return vertexCount/7;
    }

private:
    void setupBufferAttributes();
    std::array<QuadLayer, QUAD_LAYERS> layers;
    //std::unique_ptr<RenderMesh> quadMesh = nullptr;
    std::unique_ptr<Shader> fragmentShader = nullptr;
    std::unique_ptr<Shader> vertexShader = nullptr;
    GLuint shaderProgram;
    TextureManager& textureManager;
    uint32_t vertexCount = 0;
    //OGLTexture *texture = nullptr;
};


#endif //GAME_QUADRENDERER_H
