//
// Created by bison on 05-11-2017.
//

#ifndef CRPG_BIOMETYPE_H
#define CRPG_BIOMETYPE_H


#include <cstdint>
#include <string>

class BiomeType {
public:
    BiomeType();

    BiomeType(const std::string &name, uint32_t color, double minHeight, double maxHeight, double minMoisture,
              double maxMoisture);

    std::string name;
    uint32_t color;
    double minHeight;
    double maxHeight;
    double minMoisture;
    double maxMoisture;
};


#endif //CRPG_BIOMETYPE_H
