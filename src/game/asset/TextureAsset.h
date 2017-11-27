//
// Created by bison on 01-10-2017.
//

#ifndef CRPG_TEXTUREASSET_H
#define CRPG_TEXTUREASSET_H

#include <string>
#include "IAsset.h"
#include "../video/PixelBuffer.h"

class TextureAsset : public IAsset {
public:
    explicit TextureAsset(std::string filename);
    AssetStatus getStatus() override;
    void setStatus(AssetStatus as) override;
    bool load(IGame& game) override;
    bool prepare(IGame& game) override;
    std::string getName() override;
    int32_t getHandle() const {
        return handle;
    }

private:
    AssetStatus status = AssetStatus::UNLOADED;
    std::string name = "Texture";
    std::string filename;
    int32_t handle = 0;
    std::shared_ptr<PixelBuffer> buffer = nullptr;
};


#endif //CRPG_TEXTUREASSET_H
