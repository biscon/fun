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
    realWidth = screenWidth;
    realHeight = screenHeight;

    SDL_Init(SDL_INIT_VIDEO);

    // request a GL context 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // stencil size 8, so far we don't use it
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // request 4x MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // this crash on osx immediately for some reason
#ifdef __WIN32__
    glEnable(GL_MULTISAMPLE);
#endif

    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    context = SDL_GL_CreateContext(window);

    // turn off vsync
    SDL_GL_SetSwapInterval(0);

    glewExperimental = GL_TRUE;
    glewInit();

    // load texture
    //PixelBuffer *pb = new PixelBuffer("checker.png");
    //texture = new OGLTexture(atlas->getBuffer(), TextureUpdateType::STATIC);

    onViewportChanged(realWidth, realHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f, 1.0f);
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

int32_t OGLRenderer::getWidth() {
    return screenWidth;
}

int32_t OGLRenderer::getHeight() {
    return screenHeight;
}

int32_t OGLRenderer::getRealWidth() {
    return realWidth;
}

int32_t OGLRenderer::getRealHeight() {
    return realHeight;
}

void OGLRenderer::onViewportChanged(int32_t newWidth, int32_t newHeight) {
    SDL_Log("Viewport resize to %d,%d", newWidth, newHeight);

    realWidth = static_cast<uint32_t>(newWidth);
    realHeight = static_cast<uint32_t>(newHeight);

    float want_aspect;
    float real_aspect;
    float scale;

    want_aspect = (float) screenWidth / screenHeight;
    real_aspect = (float) newWidth / newHeight;

    if (SDL_fabs(want_aspect-real_aspect) < 0.0001) {
        /* The aspect ratios are the same, just scale appropriately */
        //scale = (float) newWidth / screenWidth;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = realWidth;
        viewport.h = realHeight;
        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
        //SDL_Log("Same aspect ratio");
    } else if (want_aspect > real_aspect) {
        /* We want a wider aspect ratio than is available - letterbox it */
        scale = (float) newWidth / screenWidth;
        viewport.x = 0;
        viewport.w = newWidth;
        viewport.h = (int)SDL_ceil(screenHeight * scale);
        viewport.y = (newHeight - viewport.h) / 2;
        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
        //SDL_Log("letterbox");
    } else {
        /* We want a narrower aspect ratio than is available - use side-bars */
        scale = (float) newHeight / screenHeight;
        viewport.y = 0;
        viewport.h = newHeight;
        viewport.w = (int)SDL_ceil(screenWidth * scale);
        viewport.x = (newWidth - viewport.w) / 2;
        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
        //SDL_Log("sidebars");
    }
}

/*
 * Sets a logical viewport where the renderer attempts to deliver the originally requested
 * aspect ratio by using letter/pillarboxing and scaling
 */
void OGLRenderer::setLogicalViewport() {
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
}

/*
 * Sets the viewport to the entire windows no matter what the aspect ratio might be
 * this tends to distort 2D graphics :)
 */
void OGLRenderer::setRealViewport() {
    glViewport(0, 0, realWidth, realHeight);
}

void OGLRenderer::setWindowBordered(bool bordered) {
    if(bordered)
        SDL_SetWindowBordered(window, SDL_TRUE);
    else
        SDL_SetWindowBordered(window, SDL_FALSE);
}

void OGLRenderer::maximizeWindow() {
    SDL_MaximizeWindow(window);
}

void OGLRenderer::minimizeWindow() {
    SDL_MinimizeWindow(window);
}

void OGLRenderer::restoreWindow() {
    SDL_RestoreWindow(window);
}
