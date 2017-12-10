//
// Created by bison on 07-10-2017.
//

#include <SDL_log.h>
#include <cstring>
#include "FontAsset.h"
#include "../IGame.h"

FontAsset::FontAsset(const std::string &filename, uint16 glyphWidth, uint16 glyphHeight) : filename(filename),
                                                                                               glyphWidth(glyphWidth),
                                                                                               glyphHeight(
                                                                                                       glyphHeight) {}

std::string FontAsset::getName() {
    if(filename.empty())
        return name;
    return name + " (" + filename + ")";
}

bool FontAsset::load() {
    buffer = std::unique_ptr<PixelBuffer>(new PixelBuffer());
    if(!buffer->loadFromPNG(filename))
    {
        SDL_Log("FontAsset: Failed to load '%s'", filename.c_str());
        return false;
    }

    return true;
}

void FontAsset::renderText(PixelBuffer* pb, uint16 x, uint16 y, const std::string &str)
{
    int chars_width = buffer->getWidth() / glyphWidth;
    //SDL_Log("Font buffer contains %dx%d glyphs", chars_width, chars_height);
    Rect2D rect;
    rect.width = glyphWidth;
    rect.height = glyphHeight;
    for(uint32 i = 0; i < strlen(str.c_str()); i++)
    {
        int index = (uint8_t) str[i];
        if(index > 255)
        {
            SDL_Log("Da fuck!!");
        }
        int col = index / chars_width;
        int row = index % chars_width;
        //SDL_Log("Glyph %d found at %d,%d", index, row, col);
        rect.x = row * glyphWidth;
        rect.y = col * glyphHeight;
        pb->copy(buffer.get(), &rect, x + (i * rect.width), y);
    }
}

uint32 FontAsset::measureStringWidth(const std::string &str)
{
    return (uint32) strlen(str.c_str()) * glyphWidth;
}

uint32 FontAsset::getFontHeight()
{
    return glyphHeight;
}

void FontAsset::renderTextCentered(PixelBuffer *pb,  const std::string &str)
{
    int x = (pb->getWidth() / 2) - (measureStringWidth(str) / 2);
    int y = (pb->getHeight() / 2) - (glyphHeight / 2);
    renderText(pb, (uint16_t) x , (uint16_t) y, str);
}

bool FontAsset::prepare(TextureAtlas& textureAtlas) {
    for(uint8_t chr = 0; chr <= 255; ++chr)
    {
        //SDL_Log("chr = %d", chr);
        auto pb = std::make_shared<PixelBuffer>(glyphWidth, glyphHeight);
        std::string str;
        str.push_back(chr);
        renderText(pb.get(), 0, 0, str);
        auto handle = textureAtlas.addBuffer(pb);
        glyphHandleMap[chr] = handle;
        if(chr == 255)
            break;
    }
    return true;
}

std::map<uint8_t, int32_t>& FontAsset::getGlyphHandleMap() {
    return glyphHandleMap;
}

uint16_t FontAsset::getGlyphWidth() const {
    return glyphWidth;
}

uint16_t FontAsset::getGlyphHeight() const {
    return glyphHeight;
}
