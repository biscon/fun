//
// Created by bison on 29-09-2017.
//
#include <SDL_log.h>
#include "PixelBuffer.h"
#include <cmath>
//extern "C" {
    #include "lodepng.h"

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

PixelBuffer::PixelBuffer(uint32_t width, uint32_t height) : width(width), height(height) {
    init(width, height);
}

PixelBuffer::PixelBuffer(std::string filename) {
    init(filename);
}

PixelBuffer::~PixelBuffer() {
    if(pixels != nullptr)
        free(pixels);
}

bool PixelBuffer::init(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    this->pitch = width * 4;
    this->cacheId = -1;
    pixels = (uint32_t*) malloc(height * pitch);
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

bool PixelBuffer::copy(PixelBuffer* src, Rect2D *src_rect, uint32 dst_x, uint32 dst_y)
{
    uint32 w = (uint32) src_rect->width;
    uint32 h = (uint32) src_rect->height;

    uint32 *src_ptr = src->pixels;
    src_ptr += (src_rect->y * src->width) + src_rect->x;
    uint32 *dst_ptr = pixels;
    dst_ptr += (dst_y * width) + dst_x;
    uint32 src_inc = src->width;
    uint32 dst_inc = width;

    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(uint32 y = 0; y < h; ++y)
    {
        for(uint32 x = 0; x < w; ++x)
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

bool PixelBuffer::copyToBuffer(void* dst, int32 dst_pitch)
{
    uint32 w = (uint32) width;
    uint32 h = (uint32) height;

    uint32 *src_ptr = pixels;
    uint32 *dst_ptr = (uint32*) dst;
    uint32 src_inc = width;
    uint32 dst_inc = width;

    //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
    for(uint32 y = 0; y < h; ++y)
    {
        for(uint32 x = 0; x < w; ++x)
        {
            *(dst_ptr+x) = *(src_ptr+x);
        }
        src_ptr += src_inc;
        dst_ptr += dst_inc;
    }
}

void PixelBuffer::clear(uint32_t color)
{
    if(pixels != nullptr)
        memset(pixels, color, height * pitch);
}

static inline uint32_t getPixelWrapAround(uint32_t *buffer, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    if(x < 0)
        x += width;
    if(y < 0)
        y += height;
    return buffer[((y % height)*width)+(x % width)];
}

void PixelBuffer::blur(PixelBuffer *src)
{
    uint32_t *buffer = src->pixels;
    uint8_t *row = (uint8_t*) buffer;
    int displacement = 0;
    uint32_t width = src->width;
    uint32_t height = src->height;
    uint32_t pitch = src->pitch;

    for(int32_t y = 0; y < height; ++y)
    {
        uint32_t *pixel = (uint32_t *) row;
        for(int32_t x = 0; x < width; ++x)
        {
            uint32_t p1 = getPixelWrapAround(buffer, x-1, y-1, width, height);
            uint32_t p2 = getPixelWrapAround(buffer, x, y-1, width, height);
            uint32_t p3 = getPixelWrapAround(buffer, x+1, y-1, width, height);
            uint32_t p4 = getPixelWrapAround(buffer, x-1, y, width, height);
            uint32_t p5 = getPixelWrapAround(buffer, x+1, y, width, height);
            uint32_t p6 = getPixelWrapAround(buffer, x-1, y+1, width, height);
            uint32_t p7 = getPixelWrapAround(buffer, x, y+1, width, height);
            uint32_t p8 = getPixelWrapAround(buffer, x+1, y+1, width, height);

            uint8_t r = (RED(p1) + RED(p2) + RED(p3) + RED(p4) + RED(p5) + RED(p6) + RED(p7) + RED(p8)) >> 3;
            uint8_t g = (GREEN(p1) + GREEN(p2) + GREEN(p3) + GREEN(p4) + GREEN(p5) + GREEN(p6) + GREEN(p7) + GREEN(p8)) >> 3;
            uint8_t b = (BLUE(p1) + BLUE(p2) + BLUE(p3) + BLUE(p4) + BLUE(p5) + BLUE(p6) + BLUE(p7) + BLUE(p8)) >> 3;

            *pixel++ = ARGB(OPAQUE, r, g, b);
        }
        row += pitch;
    }
}

void PixelBuffer::convertToGreyScale()
{
    auto *row = (uint8_t*) pixels;
    uint32_t *pixel = nullptr;
    uint32_t px = 0;
    uint8_t col = 0;
    for(int32_t y = 0; y < height; ++y) {
        pixel = (uint32_t *) row;
        for (int32_t x = 0; x < width; ++x) {
            px = *pixel;
            col = (uint8_t) ((RED(px) + GREEN(px) + BLUE(px))/3);
            *pixel++ = ARGB(OPAQUE, col, col, col);
        }
        row += pitch;
    }
}

void PixelBuffer::sobelEdgeDetection()
{
    auto *row = (uint8_t*) pixels;
    uint32_t *pixel = nullptr;
    uint8_t col = 0;
    unsigned int valX, valY = 0;
    for(int32_t i = 0; i < height; ++i)
    {
        pixel = (uint32_t *) row;
        for (int32_t j = 0; j < width; ++j)
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
            *pixel++ = ARGB(OPAQUE, col, col, col);
        }
        row += pitch;
    }
}

uint32_t PixelBuffer::blurAt(int32_t x, int32_t y)
{
    uint32_t p1 = getPixelWrapAround(pixels, x-1, y-1, width, height);
    uint32_t p2 = getPixelWrapAround(pixels, x, y-1, width, height);
    uint32_t p3 = getPixelWrapAround(pixels, x+1, y-1, width, height);
    uint32_t p4 = getPixelWrapAround(pixels, x-1, y, width, height);
    uint32_t p5 = getPixelWrapAround(pixels, x+1, y, width, height);
    uint32_t p6 = getPixelWrapAround(pixels, x-1, y+1, width, height);
    uint32_t p7 = getPixelWrapAround(pixels, x, y+1, width, height);
    uint32_t p8 = getPixelWrapAround(pixels, x+1, y+1, width, height);
    uint32_t p9 = getPixelWrapAround(pixels, x, y, width, height);

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
    uint32_t error = lodepng_encode32_file(filename.c_str(), (unsigned char*) pixels, width, height);
    if(error) {
        SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
        return false;
    }
    name = filename;
    return true;
}
