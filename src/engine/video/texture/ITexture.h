//
// Created by bison on 30-09-2017.
//

#ifndef CRPG_ITEXTURE_H
#define CRPG_ITEXTURE_H

#include "defs.h"

class ITexture
{
public:
    ITexture(){}
    virtual ~ITexture(){}
    virtual u32 getWidth()=0;
    virtual u32 getHeight()=0;
};

#endif //CRPG_ITEXTURE_H
