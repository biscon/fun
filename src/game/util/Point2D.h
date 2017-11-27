//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_POINT2D_H
#define CRPG_POINT2D_H


#include <cstdint>

class Point2D {
public:
    Point2D(int32_t x, int32_t y) : x(x), y(y) {}
    int32_t x,y;
};


#endif //CRPG_POINT2D_H
