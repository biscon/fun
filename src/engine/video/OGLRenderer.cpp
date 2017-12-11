//
// Created by bison on 19-11-2017.
//

#include "OGLRenderer.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include "shaders.h"
#include "OGLTexture.h"
#include "TextureAtlas.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

bool OGLRenderer::init(uint32 screenWidth, uint32 screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    // load texture
    //PixelBuffer *pb = new PixelBuffer("checker.png");
    //texture = new OGLTexture(atlas->getBuffer(), TextureUpdateType::STATIC);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_DEPTH_TEST);

    return true;
}

void OGLRenderer::shutdown() {
    if(context != nullptr)
        SDL_GL_DeleteContext(context);
    if(window != nullptr)
        SDL_DestroyWindow(window);
}

void OGLRenderer::fixedUpdate() {
    //glDrawArrays(GL_TRIANGLES, 0, 4);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("Loop OpenGL error: %d", err);
    }
    SDL_GL_SwapWindow(window);
}

void OGLRenderer::startFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OGLRenderer::renderDefaultLoadScreen(double progress) {

}
