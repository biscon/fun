//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IRENDERER_H
#define CRPG_IRENDERER_H

#include "../../defs.h"
#include "PixelBuffer.h"
#include "ITexture.h"
#include "../../system/video/TextureManager.h"
#include "../../system/video/QuadRenderer.h"
#include "../asset/FontAsset.h"

class IRenderer
{
public:
    IRenderer(){}
    virtual ~IRenderer(){}
    virtual bool init(uint32 screenWidth, uint32 screenHeight)=0;
    virtual void shutdown()=0;
    virtual void fixedUpdate()=0;
    virtual void startFrame()=0;
    virtual void renderDefaultLoadScreen(double progress)=0;
    virtual TextureManager& getTextureManager()=0;
    virtual QuadRenderer &getQuadRenderer()=0;
    virtual void renderText(std::shared_ptr<FontAsset> font, float left, float top, int32_t layer, std::string txt)=0;
};


#endif //CRPG_IRENDERER_H
