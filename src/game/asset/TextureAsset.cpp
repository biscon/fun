//
// Created by bison on 01-10-2017.
//

#include <SDL_log.h>

#include <utility>
#include <memory>
#include "TextureAsset.h"
#include "../video/PixelBuffer.h"
#include "../IGame.h"

TextureAsset::TextureAsset(std::string filename) : filename(std::move(filename))
{

}

AssetStatus TextureAsset::getStatus() {
    return status;
}

void TextureAsset::setStatus(AssetStatus as) {
    status = as;
}

bool TextureAsset::load(IGame& game) {
    buffer = std::make_shared<PixelBuffer>();
    if(!buffer->loadFromPNG(filename))
    {
        SDL_Log("TextureAsset: Failed to load '%s'", filename.c_str());
        return false;
    }
    return true;
}

bool TextureAsset::prepare(IGame& game) {
    handle = game.getRenderer()->getTextureManager().createTexture(buffer);
    return true;
}

std::string TextureAsset::getName() {
    if(filename.empty())
        return name;
    return name + " (" + filename + ")";
}
