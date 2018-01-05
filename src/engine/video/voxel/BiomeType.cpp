//
// Created by bison on 05-11-2017.
//

#include "BiomeType.h"

BiomeType::BiomeType(const std::string &name, u32 color, double minHeight, double maxHeight, double minMoisture,
                     double maxMoisture) : name(name), color(color), minHeight(minHeight), maxHeight(maxHeight),
                                           minMoisture(minMoisture), maxMoisture(maxMoisture) {}

BiomeType::BiomeType() {}
