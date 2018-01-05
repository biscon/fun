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
