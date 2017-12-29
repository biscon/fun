//
// Created by bison on 05-11-2017.
//

#ifndef CRPG_TERRAIN_H
#define CRPG_TERRAIN_H


#include "HeightMap.h"
#include "BiomeType.h"
#include <list>

class Terrain {
public:
    const double WATER_LEVEL = 0.20;
    int32_t size = 1024;

    Terrain();

    virtual ~Terrain();

    void generate();

    HeightMap *getHeightMap() const {
        return heightMap;
    }

    HeightMap *getMoistureMap() const {
        return moistureMap;
    }

    int32_t getSize() const;

    BiomeType* lookupBiomeType(double height, double moisture);

    BiomeType **getBiomeMap() const;
    BiomeType *getBiomeAt(int32_t x, int32_t y);
    bool isBiomeBorderAt(int32_t x, int32_t y);

private:
    HeightMap *heightMap = nullptr;
    HeightMap *moistureMap = nullptr;
    std::list<BiomeType*> biomeTypes;
    BiomeType **biomeMap = nullptr;
};


#endif //CRPG_TERRAIN_H
