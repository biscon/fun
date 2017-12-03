//
// Created by bison on 21-11-2017.
//

#include <SDL_log.h>
#include <algorithm>
#include "TextureManager.h"

int32_t TextureManager::createTexture(std::shared_ptr<PixelBuffer> pb) {
    // create texture and add to map
    handles[nextHandle] = std::unique_ptr<Texture>(new Texture());
    auto& tex = *handles[nextHandle];
    // assign handle
    tex.handle = nextHandle;
    tex.status = CREATED;
    tex.buffer = pb;
    nextHandle++;
    return tex.handle;
}

void TextureManager::buildTextures() {
    // create new atlas and save a reference
    atlases.push_back(std::unique_ptr<TextureAtlas>(new TextureAtlas(ATLAS_SIZE, ATLAS_SIZE)));
    TextureAtlas *atlas = atlases.back().get();

    while(true) {
        for (auto &kv : handles) {
            Texture *tex = kv.second.get();
            if (tex->status == CREATED) {
                tex->atlas = atlas;
                tex->status = BUILT;
                atlas->addBuffer(tex->buffer, tex->handle);
            }
        }
        auto all_fit = atlas->build();
        //atlas->debug();
        if(!all_fit) { // all fit, stop building atlases
            // mark the ones that didn't fit as CREATED so they will be added to next atlas instead
            for(auto id : atlas->getUnfittedIds())
            {
                handles[id]->status = CREATED;
            }
        }
        // go through the built textures and update their index
        int32_t index = 0;
        for(auto& atlas_tex : atlas->getTextures())
        {
            auto id = atlas_tex->id;
            auto tex = handles[id].get();
            tex->index = index;
            index++;
        }
        // if all fit there is no reason to make another atlas
        if(all_fit)
            break;
        // else build a new atlas and update reference, process remaining
        atlases.push_back(std::unique_ptr<TextureAtlas>(new TextureAtlas(ATLAS_SIZE, ATLAS_SIZE)));
        atlas = atlases.back().get();
    }
    SDL_Log("Texture atlasses built: %d", atlases.size());
    SDL_Log("Texture handles: %d", handles.size());
}

void TextureManager::uploadTextures() {
    SDL_Log("Uploading %d textures in %d atlases", handles.size(), atlases.size());
    //atlasTextureMap.clear();
    for(auto& atlas : atlases)
    {
        if(atlasTextureMap[atlas.get()] == nullptr) {
            textures.push_back(std::unique_ptr<OGLTexture>(new OGLTexture(atlas->getBuffer())));
            atlasTextureMap[atlas.get()] = textures.back().get();
        }
    }
    // go through each handle an assign a pointer to the texture, bit clumsy implementation
    for (auto &kv : handles) {
        Texture *tex = kv.second.get();
        tex->texture = atlasTextureMap[tex->atlas];
        //SDL_Log("Assigned texture based on atlas map");
    }

}

void TextureManager::bind(int32_t handle) {
    const auto& tex = handles.at(handle);
    if(handles.at(handle) != nullptr)
    {
        handles.at(handle)->texture->bind();
    }
}

UVRect* TextureManager::getUVRect(int32_t handle) {
    const auto& tex = handles.at(handle);
    if(handles.at(handle) != nullptr)
    {
        return &handles[handle]->atlas->getTextures().at(handles[handle]->index)->uvRect;
    }
}

Texture *TextureManager::getTexture(int32_t handle) {
    if(handles.at(handle) != nullptr)
    {
        return &*handles[handle];
    }
    return nullptr;
}

std::vector<std::unique_ptr<TextureAtlas>> &TextureManager::getAtlases() {
    return atlases;
}

void TextureManager::bindAtlas(TextureAtlas *atlas) {
    atlasTextureMap[atlas]->bind();
}
