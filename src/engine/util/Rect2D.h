//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_RECT2D_H
#define CRPG_RECT2D_H


#include "../../defs.h"

class Rect2D {
public:
    Rect2D();

    Rect2D(i32 x, i32 y, i32 width, i32 height);
    void set(i32 x, i32 y, i32 width, i32 height)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    i32 x,y,width,height;
};


#endif //CRPG_RECT2D_H
