//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_RECT2D_H
#define CRPG_RECT2D_H


#include "../../defs.h"

class Rect2D {
public:
    Rect2D();

    Rect2D(int32_t x, int32_t y, int32_t width, int32_t height);
    void set(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    int32 x,y,width,height;
};


#endif //CRPG_RECT2D_H
