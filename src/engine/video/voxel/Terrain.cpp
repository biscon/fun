//
// Created by bison on 05-11-2017.
//

#include <cstring>
#include <engine/video/PixelBuffer.h>
#include "Terrain.h"

Terrain::Terrain() {
    // low altitude
    biomeTypes.push_back(new BiomeType("Ocean", ARGB(255, 44, 44, 222), 0, WATER_LEVEL, 0, 1));
    biomeTypes.push_back(new BiomeType("Beach", ARGB(255, 255, 255, 127), WATER_LEVEL, 0.25, 0, 1));

    // medium
    biomeTypes.push_back(new BiomeType("Desert", ARGB(255, 244, 235, 121), 0.25, 0.50, 0, 0.15));
    biomeTypes.push_back(new BiomeType("Grassland", ARGB(255, 181, 211, 109), 0.25, 0.50, 0.15, 0.25));
    biomeTypes.push_back(new BiomeType("Dry forest", ARGB(255, 96, 207, 58), 0.25, 0.50, 0.25, 0.50));
    biomeTypes.push_back(new BiomeType("Wet forest", ARGB(255, 110, 211, 109), 0.25, 0.50, 0.50, 1.00));

    // high
    biomeTypes.push_back(new BiomeType("Forest", ARGB(255, 84, 164, 57), 0.50, 0.75, 0.00, 0.50));
    biomeTypes.push_back(new BiomeType("Steppe", ARGB(255, 139, 160, 22), 0.50, 0.75, 0.50, 0.75));
    biomeTypes.push_back(new BiomeType("Pine forest", ARGB(255, 42, 130, 71), 0.50, 0.75, 0.75, 1.0));

    // very high
    biomeTypes.push_back(new BiomeType("Bare rock", ARGB(255, 160, 160, 160), 0.75, 1.0, 0.0, 0.25));
    biomeTypes.push_back(new BiomeType("Rock", ARGB(255, 185, 185, 185), 0.75, 1.0, 0.25, 0.50));
    biomeTypes.push_back(new BiomeType("Snow", ARGB(255, 220, 220, 220), 0.75, 1.0, 0.50, 0.75));
    biomeTypes.push_back(new BiomeType("Wet snow", ARGB(255, 255, 255, 255), 0.75, 1.0, 0.75, 1.00));
/*
    // low altitude
    biomeTypes.push_back(new BiomeType("Ocean", ARGB(255, 22, 22, 171), 0, WATER_LEVEL, 0, 1));

    biomeTypes.push_back(new BiomeType("Subtropical Desert", ARGB(255, 255, 244, 51), WATER_LEVEL, 0.25, 0, .25));
    biomeTypes.push_back(new BiomeType("Temperate Desert", ARGB(255, 238, 220, 26), WATER_LEVEL, 0.25, .25, .75));
    biomeTypes.push_back(new BiomeType("Scorched Land", ARGB(255, 155, 155, 0), WATER_LEVEL, 0.25, .75, 1));

    // medium
    biomeTypes.push_back(new BiomeType("Desert", ARGB(255, 238, 220, 0), 0.25, 0.50, 0, 0.15));
    biomeTypes.push_back(new BiomeType("Grassland", ARGB(255, 0, 200, 0), 0.25, 0.50, 0.15, 0.5));
    biomeTypes.push_back(new BiomeType("Shrubland", ARGB(255, 44, 190, 44), 0.25, 0.50, 0.50, 0.75));
    biomeTypes.push_back(new BiomeType("Bare Land", ARGB(255, 77, 175, 50), 0.25, 0.50, 0.75, 1.00));

    // high
    biomeTypes.push_back(new BiomeType("Tropical Seasonal Forest", ARGB(255, 0, 175, 0), 0.50, 0.75, 0, 0.25));
    biomeTypes.push_back(new BiomeType("Temperate Decidous Forest", ARGB(255, 0, 155, 0), 0.50, 0.75, 0.25, 0.50));
    biomeTypes.push_back(new BiomeType("Taiga", ARGB(255, 10, 135, 20), 0.50, 0.75, 0.50, 0.75));
    biomeTypes.push_back(new BiomeType("Tundra", ARGB(255, 10, 110, 30), 0.50, 0.75, 0.75, 1.0));

    // very high
    biomeTypes.push_back(new BiomeType("Tropical Rain Forest", ARGB(255, 120, 120, 120), 0.75, 1.0, 0.0, 0.25));
    biomeTypes.push_back(new BiomeType("Temperate Rain Forest", ARGB(255, 175, 175, 175), 0.75, 1.0, 0.25, 0.50));
    biomeTypes.push_back(new BiomeType("Taiga", ARGB(255, 200, 200, 200), 0.75, 1.0, 0.50, 0.75));
    biomeTypes.push_back(new BiomeType("Snow", ARGB(255, 255, 255, 255), 0.75, 1.0, 0.75, 1.00));
*/
}

Terrain::~Terrain() {
    for(auto i = biomeTypes.begin(); i != biomeTypes.end(); i++)
    {
        delete *i;
    }
    biomeTypes.clear();
    if(moistureMap != nullptr)
        delete moistureMap;
    if(heightMap != nullptr)
        delete heightMap;
    if(biomeMap != nullptr)
    {
        delete[] biomeMap;
    }
}

void Terrain::generate() {
    if(heightMap == nullptr)
        heightMap = new HeightMap(size, size);
    if(moistureMap == nullptr)
        moistureMap = new HeightMap(size, size);

    // generate height map
    heightMap->generate(256);
    heightMap->normalize();
    heightMap->applyCircularMask();

    // generate moisture map
    moistureMap->generate(64);
    moistureMap->normalize();
    moistureMap->applyCircularMask();

    // generate biomemap
    biomeMap = new BiomeType*[size * size];
    memset(biomeMap, 0, sizeof(biomeMap));
    for(int x = 0; x < size; x++)
    {
        for(int y = 0; y < size; y++)
        {
            auto height = heightMap->getSample(x,y);
            auto moisture = moistureMap->getSample(x,y);
            BiomeType *bt = lookupBiomeType(height, moisture);
            //SDL_Log("Assigned %s biome", bt->name.c_str());
            biomeMap[y*size+x] = bt;
        }
    }
}

BiomeType* Terrain::lookupBiomeType(double height, double moisture)
{
    for(auto i = biomeTypes.begin(); i != biomeTypes.end(); i++)
    {
        BiomeType *b = *i;
        if(height >= b->minHeight && height < b->maxHeight &&
                moisture >= b->minMoisture && moisture < b->maxMoisture)
        {
            return b;
        }
    }
    return nullptr;
}

BiomeType **Terrain::getBiomeMap() const {
    return biomeMap;
}

BiomeType *Terrain::getBiomeAt(i32 x, i32 y) {
    return biomeMap[(x & (size - 1)) + (y & (size - 1)) * size];
}

i32 Terrain::getSize() const {
    return size;
}

bool Terrain::isBiomeBorderAt(i32 x, i32 y) {
    BiomeType *bt = getBiomeAt(x,y);
    // top row
    if(getBiomeAt(x-1, y-1) != bt) return true;
    if(getBiomeAt(x, y-1) != bt) return true;
    if(getBiomeAt(x+1, y-1) != bt) return true;

    // sides
    if(getBiomeAt(x-1, y) != bt) return true;
    if(getBiomeAt(x+1, y) != bt) return true;

    // bottom row
    if(getBiomeAt(x-1, y+1) != bt) return true;
    if(getBiomeAt(x, y+1) != bt) return true;
    if(getBiomeAt(x+1, y+1) != bt) return true;
    return false;
}

