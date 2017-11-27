//
// Created by bison on 19-11-2017.
//

#include "OGLRenderer.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include "shaders.h"
#include "OGLTexture.h"
#include "../../game/video/TextureAtlas.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

bool OGLRenderer::init(uint32 screenWidth, uint32 screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    textureManager = std::unique_ptr<TextureManager>(new TextureManager());
    quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(*textureManager));

    // load texture
    //PixelBuffer *pb = new PixelBuffer("checker.png");
    //texture = new OGLTexture(atlas->getBuffer(), TextureUpdateType::STATIC);

    glClearColor(0.0f, 1.0f, 0, 1.0f);
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
    quadRenderer->render(screenWidth, screenHeight);
    SDL_GL_SwapWindow(window);
}

void OGLRenderer::startFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    quadRenderer->startFrame();
}

void OGLRenderer::renderDefaultLoadScreen(double progress) {

}

TextureManager &OGLRenderer::getTextureManager() {
    return *textureManager;
}

QuadRenderer &OGLRenderer::getQuadRenderer() {
    return *quadRenderer;
}

void OGLRenderer::renderText(std::shared_ptr<FontAsset> font, float left, float top, int32_t layer, std::string txt) {
    auto len = strlen(txt.c_str());
    auto off = left;
    auto gw = font->getGlyphWidth();
    auto gh = font->getGlyphHeight();
    for(uint32 i = 0; i < len; i++)
    {
        uint8_t index = (uint8_t) txt[i];
        quadRenderer->drawTexturedQuad(font->getGlyphHandleMap()[index], off, top, off + gw, top + gh, layer);
        off += gw;
    }
}
