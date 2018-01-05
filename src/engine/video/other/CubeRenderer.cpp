//
// Created by bison on 21-11-2017.
//

#include "CubeRenderer.h"
#include "engine/video/shader/shaders.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

CubeRenderer::CubeRenderer(std::shared_ptr<TextureAtlas> textureAtlas) : textureAtlas(textureAtlas) {
    shader = std::unique_ptr<Shader>(new Shader(vertexSource3D, fragmentSource3D, nullptr));
    shader->use();

}

void CubeRenderer::render(float screenWidth, float screenHeight, double time) {
    glEnable(GL_DEPTH_TEST);
    // use shader
    shader->use();
    // bind vao
    mesh->bindVAO();

    // bind texture
    textureAtlas->bind();

    // set uniforms

    // model matrix ----------------------------------------------------------------------------------------------------
    glm::mat4 model = glm::mat4();
    //model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
    //model = glm::rotate(model, )
    model = glm::rotate(model, (float)(time) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    shader->setMat4("model", model);

    // view matrix -----------------------------------------------------------------------------------------------------
    //glm::mat4 view = glm::mat4();
    glm::mat4 view = glm::lookAt(
            glm::vec3(1.0f, 1.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    );
    shader->setMat4("view", view);

    // projection matrix -----------------------------------------------------------------------------------------------
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 1.0f, 10.0f);
    shader->setMat4("proj", proj);
    // render
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->vertices.size()));
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->elements.size()), GL_UNSIGNED_INT, 0);
    glDisable(GL_DEPTH_TEST);
}

void CubeRenderer::buildBuffers() {
    SDL_Log("Building cube mesh");
    mesh = std::unique_ptr<Mesh>(new Mesh(MeshUpdateType::STATIC));
    mesh->bindVAO();
    mesh->bindVBO();
    mesh->generateTexturedCube(textureAtlas->getUVRect(1));
    mesh->upload();
    setupBufferAttributes();
}

void CubeRenderer::setupBufferAttributes() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("setup OpenGL error: %d", err);
    }
    // setup position
    GLint posAttrib = glGetAttribLocation(shader->getShaderId(), "position");
    if(posAttrib < 0)
        SDL_Log("position attribute not found");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    // setup color
    /*
    GLint colAttrib = glGetAttribLocation(shader->getShaderId(), "color");
    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(colAttrib);
    if(colAttrib < 0)
        SDL_Log("color attribute not found");
    */

    // setup texcoords
    GLint texAttrib = glGetAttribLocation(shader->getShaderId(), "texcoord");
    if(texAttrib < 0)
        SDL_Log("texcoord attribute not found");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);
}

