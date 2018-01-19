    //
    // Created by bison on 07-10-2017.
    //

#include <SDL_log.h>
#include <cstring>
#include "FontAsset.h"
#include "../IGame.h"

FT_Library library;

b32 InitFontLoader() {
    i32 error;
    error = FT_Init_FreeType(&library);
    return error ? 1 : 0; // 1 if error
}

b32 MakeFont(Font *font, const char *path, u8 size) {
    FT_Face face;
    if (FT_New_Face(library, path, 0, &face))
        return 1;

    if (FT_Set_Pixel_Sizes(face, 0, size))
        return 1;

    font->face = face;
    font->size = size;

    LoadGlyphs(font, EN_START, EN_END);
    LoadGlyphs(font, KR_START, KR_END);

    return 0;
}

void LoadGlyphs(Font *font, u32 cpStart, u32 cpEnd) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (u32 cp = cpStart; cp <= cpEnd; cp += 1) {
        if (FT_Load_Char(font->face, cp, FT_LOAD_RENDER)) {
            printf("Failed to load glyph for codepoint: 0x%x\n", cp);
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            font->face->glyph->bitmap.width,
            font->face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font->face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character ch = {
            texture,
            static_cast<GLuint>(font->face->glyph->advance.x >> 6),
            glm::ivec2(font->face->glyph->bitmap.width, font->face->glyph->bitmap.rows),
            glm::ivec2(font->face->glyph->bitmap_left,  font->face->glyph->bitmap_top)
        };

        font->characterMap.insert(std::pair<u32, struct Character>(cp, ch));
    }
}

FontAsset::FontAsset(const std::string &filename, u16 glyphWidth, u16 glyphHeight) : filename(filename),
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

void FontAsset::renderText(PixelBuffer* pb, u16 x, u16 y, const std::string &str)
{
    int chars_width = buffer->getWidth() / glyphWidth;
    //SDL_Log("Font buffer contains %dx%d glyphs", chars_width, chars_height);
    Rect2D rect;
    rect.width = glyphWidth;
    rect.height = glyphHeight;
    for(u32 i = 0; i < strlen(str.c_str()); i++)
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

u32 FontAsset::measureStringWidth(const std::string &str)
{
    return (u32) strlen(str.c_str()) * glyphWidth;
}

u32 FontAsset::getFontHeight()
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

std::map<uint8_t, i32>& FontAsset::getGlyphHandleMap() {
    return glyphHandleMap;
}

uint16_t FontAsset::getGlyphWidth() const {
    return glyphWidth;
}

uint16_t FontAsset::getGlyphHeight() const {
    return glyphHeight;
}
