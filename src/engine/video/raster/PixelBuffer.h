//
// Created by bison on 29-09-2017.
//

#ifndef CRPG_PIXELBUFFER_H
#define CRPG_PIXELBUFFER_H

#define VAL_OPAQUE 0xFF
#define VAL_TRANSPARENT 0

#define ALPHA(rgb) (uint8_t)(rgb >> 24)
#define BLUE(rgb)   (uint8_t)(rgb >> 16)
#define GREEN(rgb) (uint8_t)(rgb >> 8)
#define RED(rgb)  (uint8_t)(rgb)

#define UNMULTIPLY(color, alpha) ((0xFF * color) / alpha)
#define BLEND(back, front, alpha) ((front * alpha) + (back * (255 - alpha))) / 255
#define ARGB(a, r, g, b) (a << 24) | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF)

#include <string>
#include "defs.h"
#include "engine/util/Rect2D.h"

class PixelBuffer {
public:
    PixelBuffer();

    PixelBuffer(u32 width, u32 height);
    explicit PixelBuffer(std::string filename);
    virtual ~PixelBuffer();
    bool loadFromPNG(std::string filename);
    bool saveToPNG(std::string filename);
    bool copy(PixelBuffer* src, Rect2D *src_rect, u32 dst_x, u32 dst_y);
    bool copyToBuffer(void* dst, i32 dst_pitch);
    void clear(u32 color);
    u32 getWidth() const { return width; }
    u32 getHeight() const { return height; }
    u32 *getPixels() const { return pixels; }
    u32 getPitch() const { return pitch; }
    std::string getName() const { return name; }
    void blur(PixelBuffer *src);
    void convertToGreyScale();
    void sobelEdgeDetection();
    u32 blurAt(i32 x, i32 y);
    PixelBuffer *clone();

private:
    std::string name = "PixelBuffer";
    u32 width, height, pitch;
    u32 *pixels = nullptr;
    bool hasAlpha;
    i16 cacheId;

    // for rigmor zobel edge detection ;)
    unsigned int GX [3][3];
    unsigned int GY [3][3];


    bool init(u32 width, u32 height);
    bool init(std::string);
};


#endif //CRPG_PIXELBUFFER_H
