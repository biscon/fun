//
// Created by bison on 07-10-2017.
//

#ifndef CRPG_FONTASSET_H
#define CRPG_FONTASSET_H

#include <string>
#include <map>
#include <memory>
#include "../video/PixelBuffer.h"
#include "../video/TextureAtlas.h"

class FontAsset {
public:
    FontAsset(const std::string &filename, uint16_t glyphWidth, uint16_t glyphHeight);
    bool load();
    std::string getName();
    uint32_t measureStringWidth(const std::string &str);
    void renderText(PixelBuffer *pb, uint16 x, uint16 y, const std::string &str);
    uint32_t getFontHeight();
    void renderTextCentered(PixelBuffer *pb, const std::string &str);
    bool prepare(TextureAtlas& textureAtlas);
    std::map<uint8_t, int32_t> &getGlyphHandleMap();
    uint16_t getGlyphWidth() const;
    uint16_t getGlyphHeight() const;

private:
    std::string name = "Font";
    std::string filename;
    std::unique_ptr<PixelBuffer> buffer = nullptr;
    std::map<uint8_t, int32_t> glyphHandleMap;
    uint16 glyphWidth;
    uint16 glyphHeight;
};


#endif //CRPG_FONTASSET_H
