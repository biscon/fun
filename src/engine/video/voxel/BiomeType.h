//
// Created by bison on 05-11-2017.
//

#ifndef CRPG_BIOMETYPE_H
#define CRPG_BIOMETYPE_H


#include "defs.h"
#include <string>

class BiomeType {
public:
    BiomeType();

    BiomeType(const std::string &name, u32 color, double minHeight, double maxHeight, double minMoisture,
              double maxMoisture);

    std::string name;
    u32 color;
    double minHeight;
    double maxHeight;
    double minMoisture;
    double maxMoisture;
};


#endif //CRPG_BIOMETYPE_H
