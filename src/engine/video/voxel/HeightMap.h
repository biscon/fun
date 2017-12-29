//
// Created by bison on 15-10-2017.
//

#ifndef CRPG_HEIGHTMAP_H
#define CRPG_HEIGHTMAP_H


#include <SDL_log.h>
#include <cstdlib>
#include <defs.h>

class Point2D {
public:
    Point2D(int32_t x, int32_t y) : x(x), y(y) {}
    int32_t x,y;
};

class HeightMap {
public:
    HeightMap(int32_t width, int32_t height);

    inline double getSampleWrap(int32 x, int32 y)
    {
        return data[(x & (width - 1)) + (y & (height - 1)) * width];
    }

    inline void setSampleWrap(int32 x, int32 y, double val)
    {
        data[(x & (width - 1)) + (y & (height - 1)) * width] = val;
    }

    inline double getSampleClip(int32 x, int32 y)
    {
        if(x > 0 && x < width && y > 0 && y < height)
            return data[y*width+x];
        else
            return 0.0;
    }

    inline void setSampleClip(int32 x, int32 y, double val)
    {
        if(x > 0 && x < width && y > 0 && y < height)
            data[y*width+x] = val;
    }

    inline double getSample(int32 x, int32 y)
    {
        if(wrapCoords)
            return getSampleWrap(x,y);
        else
            return getSampleClip(x,y);
    }

    inline void setSample(int32 x, int32 y, double val)
    {
        if(wrapCoords)
            setSampleWrap(x,y,val);
        else
            setSampleClip(x,y,val);
    }

    int32_t getWidth() const;
    int32_t getHeight() const;

    const Point2D &getHighPoint() const;
    void setHighPoint(const Point2D &highPoint);
    const Point2D &getLowPoint() const;
    void setLowPoint(const Point2D &lowPoint);
    double getLowest() const;
    double getHighest() const;

    void generate(int featuresize);
    void applyCircularMask();
    void normalize();


private:
    double* data;
    int32 width, height;
    uint32 seed;
    double minVal = -1;
    double maxVal = 1;
    double lowest = 0;
    double highest = 0;
    Point2D highPoint = {0,0};
    Point2D lowPoint = {0,0};
    bool wrapCoords = true;

    double frand(double fMin, double fMax) {
        double f = (double) rand() / (double) RAND_MAX;
        return fMin + f * (fMax - fMin);
    }

    void sampleSquare(int x, int y, int size, double value);
    void sampleDiamond(int x, int y, int size, double value);
    void diamondSquare(int stepsize, double scale);
    void findHighLowPoints();
};


#endif //CRPG_HEIGHTMAP_H
