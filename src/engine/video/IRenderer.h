//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IRENDERER_H
#define CRPG_IRENDERER_H

#include "../../defs.h"
#include "engine/video/raster/PixelBuffer.h"
#include "engine/video/texture/ITexture.h"
#include "../asset/FontAsset.h"

class IRenderer
{
public:
    IRenderer(){}
    virtual ~IRenderer(){}
    virtual bool init(u32 screenWidth, u32 screenHeight)=0;
    virtual void shutdown()=0;
    virtual void render()=0;
    virtual void startFrame()=0;
    virtual void renderDefaultLoadScreen(double progress)=0;
    virtual i32 getWidth()=0;
    virtual i32 getHeight()=0;
    virtual i32 getRealWidth()=0;
    virtual i32 getRealHeight()=0;
    virtual void onViewportChanged(i32 newWidth, i32 newHeight)=0;
    virtual void setLogicalViewport()=0;
    virtual void setRealViewport()=0;
    virtual void setWindowBordered(bool bordered)=0;
    virtual void maximizeWindow()=0;
    virtual void minimizeWindow()=0;
    virtual void restoreWindow()=0;
};


#endif //CRPG_IRENDERER_H
