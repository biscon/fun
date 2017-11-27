//
// Created by bison on 29-09-2017.
//

#ifndef CRPG_PIXELBUFFER_H
#define CRPG_PIXELBUFFER_H

#define OPAQUE 0xFF
#define TRANSPARENT 0

#define ALPHA(rgb) (uint8_t)(rgb >> 24)
#define BLUE(rgb)   (uint8_t)(rgb >> 16)
#define GREEN(rgb) (uint8_t)(rgb >> 8)
#define RED(rgb)  (uint8_t)(rgb)

#define UNMULTIPLY(color, alpha) ((0xFF * color) / alpha)
#define BLEND(back, front, alpha) ((front * alpha) + (back * (255 - alpha))) / 255
#define ARGB(a, r, g, b) (a << 24) | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF)

#include <string>
#include "../../defs.h"
#include "../util/Rect2D.h"

class PixelBuffer {
public:
    PixelBuffer();

    PixelBuffer(uint32_t width, uint32_t height);
    explicit PixelBuffer(std::string filename);
    virtual ~PixelBuffer();
    bool loadFromPNG(std::string filename);
    bool saveToPNG(std::string filename);
    bool copy(PixelBuffer* src, Rect2D *src_rect, uint32_t dst_x, uint32_t dst_y);
    bool copyToBuffer(void* dst, int32 dst_pitch);
    void clear(uint32_t color);
    uint32 getWidth() const { return width; }
    uint32 getHeight() const { return height; }
    uint32 *getPixels() const { return pixels; }
    uint32_t getPitch() const { return pitch; }
    std::string getName() const { return name; }
    void blur(PixelBuffer *src);
    void convertToGreyScale();
    void sobelEdgeDetection();
    uint32_t blurAt(int32_t x, int32_t y);
    PixelBuffer *clone();

private:
    std::string name = "PixelBuffer";
    uint32 width, height, pitch;
    uint32 *pixels = nullptr;
    bool hasAlpha;
    int16 cacheId;

    // for rigmor zobel edge detection ;)
    unsigned int GX [3][3];
    unsigned int GY [3][3];


    bool init(uint32_t width, uint32_t height);
    bool init(std::string);
};


#endif //CRPG_PIXELBUFFER_H
