//
// Created by bison on 09-12-2017.
//

#include "FontRenderer.h"

FontRenderer::FontRenderer() {
    textureAtlas = std::make_shared<TextureAtlas>(1024, 1024, false);
    quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(textureAtlas));
}

void FontRenderer::render(float screenWidth, float screenHeight) {
    quadRenderer->render(screenWidth, screenHeight);
}

void FontRenderer::startFrame() {
    quadRenderer->startFrame();
}

int32_t FontRenderer::addFont(std::string filename, uint16_t glyphWidth, uint16_t glyphHeight) {
    auto handle = nextHandle;
    nextHandle++;
    fonts[handle] = std::unique_ptr<FontAsset>(new FontAsset(filename, glyphWidth, glyphHeight));
    return handle;
}

bool FontRenderer::load(IGame &game) {
    for(auto const& kv : fonts)
    {
        kv.second->load();
        kv.second->prepare(*textureAtlas);
    }
    textureAtlas->build();
    return true;
}

bool FontRenderer::prepare(IGame &game) {
    textureAtlas->upload();
    quadRenderer->buildBuffers();
    return true;
}

void FontRenderer::renderText(int32_t handle, float left, float top, std::string txt) {
    FontAsset *font = fonts.at(handle).get();
    auto len = strlen(txt.c_str());
    auto off = left;
    auto gw = font->getGlyphWidth();
    auto gh = font->getGlyphHeight();
    for(uint32 i = 0; i < len; i++)
    {
        uint8_t index = (uint8_t) txt[i];
        quadRenderer->drawTexturedQuad(font->getGlyphHandleMap()[index], off, top, off + gw, top + gh, 0);
        off += gw;
    }
}


