//
// Created by bison on 15-10-2017.
//

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <cstring>
#include <engine/util/math_util.h>
#include "HeightMap.h"

HeightMap::HeightMap(i32 width, i32 height) : width(width), height(height) {
    data = new double[width * height];
}

void HeightMap::generate(int featuresize) {
    seed = static_cast<unsigned int>(time(nullptr));
    srand(seed);
    memset(data, 0, sizeof(data));

    for( int y = 0; y < height; y += featuresize) {
        for (int x = 0; x < width; x += featuresize) {
            setSample(x, y, frand(minVal,
                                  maxVal));  //IMPORTANT: frand() is a random function that returns a value between -1 and 1.
        }
    }
    int samplesize = featuresize;
    double scale = 1.0;

    while (samplesize > 1)
    {
        diamondSquare(samplesize, scale);
        samplesize /= 2;
        scale /= 2.0;
    }
    findHighLowPoints();
}

void HeightMap::sampleSquare(int x, int y, int size, double value)
{
    int hs = size / 2;

    // a     b
    //
    //    x
    //
    // c     d

    double a = getSample(x - hs, y - hs);
    double b = getSample(x + hs, y - hs);
    double c = getSample(x - hs, y + hs);
    double d = getSample(x + hs, y + hs);

    setSample(x, y, ((a + b + c + d) / 4.0) + value);

}

void HeightMap::sampleDiamond(int x, int y, int size, double value)
{
    int hs = size / 2;

    //   c
    //
    //a  x  b
    //
    //   d

    double a = getSample(x - hs, y);
    double b = getSample(x + hs, y);
    double c = getSample(x, y - hs);
    double d = getSample(x, y + hs);

    setSample(x, y, ((a + b + c + d) / 4.0) + value);
}

void HeightMap::diamondSquare(int stepsize, double scale)
{

    int halfstep = stepsize / 2;
    for (int y = halfstep; y < height + halfstep; y += stepsize)
    {
        for (int x = halfstep; x < width + halfstep; x += stepsize)
        {
            sampleSquare(x, y, stepsize, frand(minVal, maxVal) * scale);
        }
    }
    for (int y = 0; y < height; y += stepsize)
    {
        for (int x = 0; x < width; x += stepsize)
        {
            sampleDiamond(x + halfstep, y, stepsize, frand(minVal, maxVal) * scale);
            sampleDiamond(x, y + halfstep, stepsize, frand(minVal, maxVal) * scale);
        }
    }
}

i32 HeightMap::getWidth() const {
    return width;
}

i32 HeightMap::getHeight() const {
    return height;
}

void HeightMap::normalize() {
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            auto s = getSample(x,y);
            setSample(x,y, remapDouble(lowest, highest, 0, 1, s));
        }
    }
}

void HeightMap::findHighLowPoints() {
    lowest = std::numeric_limits<double>::max();
    highest = std::numeric_limits<double>::min();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            auto s = getSample(x,y);
            if(s < lowest) {
                lowest = s;
                lowPoint.x = x;
                lowPoint.y = y;
            }
            if(s > highest) {
                highest = s;
                highPoint.x = x;
                highPoint.y = y;
            }
        }
    }
    SDL_Log("Found lowest point %f at %d,%d and higest point %f at %d,%d", lowest, lowPoint.x, lowPoint.y, highest, highPoint.x, highPoint.y);
}

const Point2D &HeightMap::getHighPoint() const {
    return highPoint;
}

void HeightMap::setHighPoint(const Point2D &highPoint) {
    HeightMap::highPoint = highPoint;
}

const Point2D &HeightMap::getLowPoint() const {
    return lowPoint;
}

void HeightMap::setLowPoint(const Point2D &lowPoint) {
    HeightMap::lowPoint = lowPoint;
}

double HeightMap::getLowest() const {
    return lowest;
}

double HeightMap::getHighest() const {
    return highest;
}

void HeightMap::applyCircularMask()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            auto s = getSample(x,y);
            double distance_x = abs((double) x - (double) width * 0.5);
            double distance_y = abs((double) y - (double) height * 0.5);
            double distance = sqrt(distance_x*distance_x + distance_y*distance_y); // circular mask

            double max_width = (double) width * 0.5;
            double delta = distance / max_width;
            double gradient = delta * delta;
            /*
            if(gradient > 1)
                gradient = 1;
            setSample(x,y, 1-gradient);
            */
            auto val = s - gradient;
            if(val < 0)
                val = 0;
            setSample(x, y, val);
        }
    }
}