//
// Created by bison on 21-11-2017.
//

#include "QuadRenderer.h"
#include "shaders.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

QuadRenderer::QuadRenderer(TextureManager &textureManager) : textureManager(textureManager) {
    fragmentShader = std::unique_ptr<Shader>(new Shader(fragmentSource, GL_FRAGMENT_SHADER));
    fragmentShader->compile();

    vertexShader = std::unique_ptr<Shader>(new Shader(vertexSource, GL_VERTEX_SHADER));
    vertexShader->compile();

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader->getShaderId());
    glAttachShader(shaderProgram, fragmentShader->getShaderId());
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    SDL_Log("Build and bound shader program");
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("OpenGL error: %d", err);
    }

}

void QuadRenderer::render(float screenWidth, float screenHeight) {
    // upload meshes
    bool did_upload = false;
    for(const auto& layer : layers) {
        for (const auto &kv : layer.meshMap) {
            if (!kv.second->vertices.empty()) {
                kv.second->upload();
                did_upload = true;
            }
        }
    }

    if(!did_upload) // nothing to render
    {
        return;
    }

    vertexCount = 0;
    for(const auto& layer : layers) {
        for (const auto &kv : layer.meshMap) {
            if (kv.second->vertices.empty())    // skip empty meshes
                continue;
            vertexCount += kv.second->vertices.size();
            //SDL_Log("Performing render");
            // use shader
            glUseProgram(shaderProgram);
            // bind vao
            kv.second->bindVAO();

            // bind texture
            textureManager.bindAtlas(kv.first);

            glm::mat4 model = glm::mat4();
            //model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
            GLint uniModel = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

            glm::mat4 view = glm::mat4();
            //view = glm::translate(view, glm::vec3(640.0f, 310.0f, .0f));
            /*
            glm::mat4 view = glm::lookAt(
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
            );
            */
            GLint uniView = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

            //glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 1.0f, 10.0f);
            glm::mat4 proj = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f);
            GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
            glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(kv.second->elements.size()), GL_UNSIGNED_INT, 0);
        }
    }
}

void QuadRenderer::startFrame() {
    for(const auto& layer : layers) {
        for (const auto &kv : layer.meshMap) {
            kv.second->clear();
        }
    }
}

void QuadRenderer::drawTexturedQuad(int32_t handle, float left, float top, float right, float bottom, int32_t layer) {
    Texture *texture = textureManager.getTexture(handle);
    if(texture == nullptr)
        return;
    if(layer < 0)
        layer = 0;
    if(layer > QUAD_LAYERS)
        layer = QUAD_LAYERS - 1;
    layers[layer].meshMap[texture->atlas]->addTexturedQuad(left, top, right, bottom, *textureManager.getUVRect(handle));
}

void QuadRenderer::buildBuffers() {
    SDL_Log("Building buffers for atlases");
    for(auto& layer : layers)
    {
        if(!layer.meshMap.empty())
        {
            layer.meshMap.clear();
        }
        for(const auto& atlas : textureManager.getAtlases())
        {
            layer.meshMap[atlas.get()] = std::unique_ptr<RenderMesh>(new RenderMesh());
            layer.meshMap[atlas.get()]->bindVAO();
            layer.meshMap[atlas.get()]->bindVBO();
            setupBufferAttributes();
        }
    }


}

void QuadRenderer::setupBufferAttributes() {
    //glUseProgram(shaderProgram);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("setup OpenGL error: %d", err);
    }
    // setup position
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    if(posAttrib < 0)
        SDL_Log("position attribute not found");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    // setup color
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(colAttrib);
    if(colAttrib < 0)
        SDL_Log("color attribute not found");

    // setup texcoords
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    if(texAttrib < 0)
        SDL_Log("texcoord attribute not found");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);
}

