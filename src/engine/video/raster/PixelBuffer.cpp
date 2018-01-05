//
// Created by bison on 29-09-2017.
//
#include <SDL_log.h>
#include "PixelBuffer.h"
#include <cmath>
//extern "C" {
    #include "engine/asset/lodepng.h"

//}

PixelBuffer::PixelBuffer() {
    //Sobel Horizontal Mask
    GX[0][0] = 1; GX[0][1] = 0; GX[0][2] = -1;
    GX[1][0] = 2; GX[1][1] = 0; GX[1][2] = -2;
    GX[2][0] = 1; GX[2][1] = 0; GX[2][2] = -1;

//Sobel Vertical Mask
    GY[0][0] =  1; GY[0][1] = 2; GY[0][2] =   1;
    GY[1][0] =  0; GY[1][1] = 0; GY[1][2] =   0;
    GY[2][0] = -1; GY[2][1] =-2; GY[2][2] =  -1;
}

PixelBuffer::PixelBuffer(u32 width, u32 height) : width(width), height(height) {
    init(width, height);
}

PixelBuffer::PixelBuffer(std::string filename) {
    init(filename);
}

PixelBuffer::~PixelBuffer() {
    if(pixels != nullptr)
        free(pixels);
}

bool PixelBuffer::init(u32 width, u32 height)
{
    this->width = width;
    this->height = height;
    this->pitch = width * 4;
    this->cacheId = -1;
    pixels = (u32*) malloc(height * pitch);
    clear(0);
    return true;
}

bool PixelBuffer::init(std::string filename)
{
    name = filename;
    unsigned error;
    unsigned char* image;
    unsigned width, height;
    error = lodepng_decode32_file(&image, &width, &height, filename.c_str());
    if(error) {
        SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }
    if(!init(width, height))
        return false;

    memcpy(pixels, image, pitch * height);
    free(image);
    return true;
}

bool PixelBuffer::loadFromPNG(std::string filename) {
    return init(filename);
}

bool PixelBuffer::copy(PixelBuffer* src, Rect2D *src_rect, u32 dst_x, u32 dst_y)
{
    u32 w = (u32) src_rect->width;
    u32 h = (u32) src_rect->height;

    u32 *src_ptr = src->pixels;
    src_ptr += (src_rect->y * src->width) + src_rect->x;
    u32 *dst_ptr = pixels;
    dst_ptr += (dst_y * width) + dst_x;
    u32 src_inc = src->width;
    u32 dst_inc = width;

    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(u32 y = 0; y < h; ++y)
    {
        for(u32 x = 0; x < w; ++x)
        {
            //if(*(src_ptr+x) != 0)
            //{
                *(dst_ptr+x) = *(src_ptr+x);
            //}
        }
        src_ptr += src_inc;
        dst_ptr += dst_inc;
    }
}

bool PixelBuffer::copyToBuffer(void* dst, i32 dst_pitch)
{
    u32 w = (u32) width;
    u32 h = (u32) height;

    u32 *src_ptr = pixels;
    u32 *dst_ptr = (u32*) dst;
    u32 src_inc = width;
    u32 dst_inc = width;

    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(u32 y = 0; y < h; ++y)
    {
        for(u32 x = 0; x < w; ++x)
        {
            *(dst_ptr+x) = *(src_ptr+x);
        }
        src_ptr += src_inc;
        dst_ptr += dst_inc;
    }
}

void PixelBuffer::clear(u32 color)
{
    if(pixels != nullptr)
        memset(pixels, color, height * pitch);
}

static inline u32 getPixelWrapAround(u32 *buffer, i32 x, i32 y, u32 width, u32 height)
{
    if(x < 0)
        x += width;
    if(y < 0)
        y += height;
    return buffer[((y % height)*width)+(x % width)];
}

void PixelBuffer::blur(PixelBuffer *src)
{
    u32 *buffer = src->pixels;
    uint8_t *row = (uint8_t*) buffer;
    int displacement = 0;
    u32 width = src->width;
    u32 height = src->height;
    u32 pitch = src->pitch;

    for(i32 y = 0; y < height; ++y)
    {
        u32 *pixel = (u32 *) row;
        for(i32 x = 0; x < width; ++x)
        {
            u32 p1 = getPixelWrapAround(buffer, x-1, y-1, width, height);
            u32 p2 = getPixelWrapAround(buffer, x, y-1, width, height);
            u32 p3 = getPixelWrapAround(buffer, x+1, y-1, width, height);
            u32 p4 = getPixelWrapAround(buffer, x-1, y, width, height);
            u32 p5 = getPixelWrapAround(buffer, x+1, y, width, height);
            u32 p6 = getPixelWrapAround(buffer, x-1, y+1, width, height);
            u32 p7 = getPixelWrapAround(buffer, x, y+1, width, height);
            u32 p8 = getPixelWrapAround(buffer, x+1, y+1, width, height);

            uint8_t r = (RED(p1) + RED(p2) + RED(p3) + RED(p4) + RED(p5) + RED(p6) + RED(p7) + RED(p8)) >> 3;
            uint8_t g = (GREEN(p1) + GREEN(p2) + GREEN(p3) + GREEN(p4) + GREEN(p5) + GREEN(p6) + GREEN(p7) + GREEN(p8)) >> 3;
            uint8_t b = (BLUE(p1) + BLUE(p2) + BLUE(p3) + BLUE(p4) + BLUE(p5) + BLUE(p6) + BLUE(p7) + BLUE(p8)) >> 3;

            *pixel++ = ARGB(VAL_OPAQUE, r, g, b);
        }
        row += pitch;
    }
}

void PixelBuffer::convertToGreyScale()
{
    auto *row = (uint8_t*) pixels;
    u32 *pixel = nullptr;
    u32 px = 0;
    uint8_t col = 0;
    for(i32 y = 0; y < height; ++y) {
        pixel = (u32 *) row;
        for (i32 x = 0; x < width; ++x) {
            px = *pixel;
            col = (uint8_t) ((RED(px) + GREEN(px) + BLUE(px))/3);
            *pixel++ = ARGB(VAL_OPAQUE, col, col, col);
        }
        row += pitch;
    }
}

void PixelBuffer::sobelEdgeDetection()
{
    auto *row = (uint8_t*) pixels;
    u32 *pixel = nullptr;
    uint8_t col = 0;
    unsigned int valX, valY = 0;
    for(i32 i = 0; i < height; ++i)
    {
        pixel = (u32 *) row;
        for (i32 j = 0; j < width; ++j)
        {
            //setting the pixels around the border to 0,
            //because the Sobel kernel cannot be allied to them
            if ((i==0)||(i==width-1)||(j==0)||(j==height-1))
            {
                valX=0;
                valY=0;
            }
            else
            {
                //calculating the X and Y convolutions
                valX = valY = 0;
                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        valX = valX + RED(getPixelWrapAround(pixels, i+x, j+y, width, height)) * GX[1+x][1+y];
                        valY = valY + RED(getPixelWrapAround(pixels, i+x, j+y, width, height)) * GY[1+x][1+y];
                    }
                }
            }

            //Gradient magnitude
            col = (uint8_t) sqrt(valX*valX + valY*valY);

            //setting the new pixel value
            *pixel++ = ARGB(VAL_OPAQUE, col, col, col);
        }
        row += pitch;
    }
}

u32 PixelBuffer::blurAt(i32 x, i32 y)
{
    u32 p1 = getPixelWrapAround(pixels, x-1, y-1, width, height);
    u32 p2 = getPixelWrapAround(pixels, x, y-1, width, height);
    u32 p3 = getPixelWrapAround(pixels, x+1, y-1, width, height);
    u32 p4 = getPixelWrapAround(pixels, x-1, y, width, height);
    u32 p5 = getPixelWrapAround(pixels, x+1, y, width, height);
    u32 p6 = getPixelWrapAround(pixels, x-1, y+1, width, height);
    u32 p7 = getPixelWrapAround(pixels, x, y+1, width, height);
    u32 p8 = getPixelWrapAround(pixels, x+1, y+1, width, height);
    u32 p9 = getPixelWrapAround(pixels, x, y, width, height);

    uint8_t r = (uint8_t) ((RED(p1) + RED(p2) + RED(p3) + RED(p4) + RED(p5) + RED(p6) + RED(p7) + RED(p8) + RED(p9)) / 9);
    uint8_t g = (uint8_t) ((GREEN(p1) + GREEN(p2) + GREEN(p3) + GREEN(p4) + GREEN(p5) + GREEN(p6) + GREEN(p7) + GREEN(p8) + GREEN(p9)) / 9);
    uint8_t b = (uint8_t) ((BLUE(p1) + BLUE(p2) + BLUE(p3) + BLUE(p4) + BLUE(p5) + BLUE(p6) + BLUE(p7) + BLUE(p8) + BLUE(p9)) / 9);

    return ARGB(255, r, g, b);
}

PixelBuffer *PixelBuffer::clone() {
    PixelBuffer *new_pb = new PixelBuffer(width, height);
    memcpy(new_pb->pixels, pixels, height*pitch);
    return new_pb;
}

bool PixelBuffer::saveToPNG(std::string filename) {
    u32 error = lodepng_encode32_file(filename.c_str(), (unsigned char*) pixels, width, height);
    if(error) {
        SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }
    name = filename;
    return true;
}
