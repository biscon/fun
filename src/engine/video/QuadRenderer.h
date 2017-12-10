//
// Created by bison on 21-11-2017.
//

#ifndef GAME_QUADRENDERER_H
#define GAME_QUADRENDERER_H

#include "Shader.h"
#include "RenderMesh.h"
#include "OGLTexture.h"

class QuadRenderer {
public:
    explicit QuadRenderer(std::shared_ptr<TextureAtlas> textureAtlas);

    void drawTexturedQuad(int32_t handle, float left, float top, float right, float bottom, int32_t layer);
    void render(float screenWidth, float screenHeight);
    void startFrame();
    void buildBuffers();

    uint32_t getVertexCount() const {
        return vertexCount/7;
    }

private:
    void setupBufferAttributes();
    std::unique_ptr<Shader> fragmentShader = nullptr;
    std::unique_ptr<Shader> vertexShader = nullptr;
    std::unique_ptr<RenderMesh> mesh = nullptr;
    std::shared_ptr<TextureAtlas> textureAtlas;
    GLuint shaderProgram;
    uint32_t vertexCount = 0;
    //OGLTexture *texture = nullptr;
};


#endif //GAME_QUADRENDERER_H
