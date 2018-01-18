//
// Created by bison on 07-10-2017.
//

#ifndef CRPG_FONTASSET_H
#define CRPG_FONTASSET_H

#include <string>
#include <map>
#include <memory>
#include "engine/video/raster/PixelBuffer.h"
#include "engine/video/texture/TextureAtlas.h"

#include "../../glm/glm.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

#define EN 0x01
#define EN_START 0x00
#define EN_END   0x80

#define KR 0x02
#define KR_START 0xAC00
#define KR_END   0xD7A3

struct Character {
    GLuint textureId;
    GLuint advance;
    glm::ivec2 size;
    glm::ivec2 bearing;
};

struct Font {
    std::map<u32, struct Character> characterMap;
    FT_Face face;
    u8 size;
};

b32 InitFontLoader();
b32 MakeFont(Font *font, const char *path, u8 size);
void LoadGlyphs(Font *font, u32 cpStart, u32 cpEnd);
#define LoadGlyph(f, cp) LoadGlyphs(f, cp, 0)

class FontAsset {
public:
    FontAsset(const std::string &filename, uint16_t glyphWidth, uint16_t glyphHeight);
    bool load();
    std::string getName();
    u32 measureStringWidth(const std::string &str);
    void renderText(PixelBuffer *pb, u16 x, u16 y, const std::string &str);
    u32 getFontHeight();
    void renderTextCentered(PixelBuffer *pb, const std::string &str);
    bool prepare(TextureAtlas& textureAtlas);
    std::map<uint8_t, i32> &getGlyphHandleMap();
    uint16_t getGlyphWidth() const;
    uint16_t getGlyphHeight() const;

private:
    std::string name = "Font";
    std::string filename;
    std::unique_ptr<PixelBuffer> buffer = nullptr;
    std::map<uint8_t, i32> glyphHandleMap;
    u16 glyphWidth;
    u16 glyphHeight;
};


#endif //CRPG_FONTASSET_H
