//
// Created by bison on 20-11-2017.
//

#include <algorithm>
#include <SDL.h>
#include "TextureAtlas.h"
#include "../util/math_util.h"


TextureAtlas::TextureAtlas(uint32_t width, uint32_t height) : width(width), height(height) {

}

int32_t TextureAtlas::addBuffer(std::shared_ptr<PixelBuffer> pb) {
    auto handle = nextHandle;
    idMap[pb] = handle;
    buffers.push_back(pb);
    nextHandle++;
    return handle;
}

struct image_size_sort_comparator
{
    inline bool operator() (const std::shared_ptr<PixelBuffer>& pb1, const std::shared_ptr<PixelBuffer>& pb2)
    {
        return (pb1->getWidth()*pb1->getHeight() > pb2->getWidth()*pb2->getHeight());
    }
};

bool TextureAtlas::build() {
    atlas = std::unique_ptr<PixelBuffer>(new PixelBuffer(width, height));
    root = std::unique_ptr<AtlasNode>(new AtlasNode());
    root->rect.set(0, 0, width-1, height-1);
    textureMap.clear();
    unfittedIds.clear();

    Rect2D src_rect;
    UVRect uv_rect;

    std::sort(buffers.begin(), buffers.end(), image_size_sort_comparator());
    for(auto i = buffers.begin(); i != buffers.end(); ++i)
    {
        auto node = root->insert(**i);
        auto pb = *i;
        if(node != nullptr)
        {
            node->buffer = pb;
            src_rect.set(0, 0, pb->getWidth(), pb->getHeight());
            atlas->copy(pb.get(), &src_rect, static_cast<uint32_t>(node->rect.left), static_cast<uint32_t>(node->rect.top));
            // calculate texture coordinates (opengl uv space)
            uv_rect.left = remapFloat(0, (float) width, 0, 1, (float) node->rect.left);
            uv_rect.right = remapFloat(0, (float) width, 0, 1, (float) node->rect.right+1);
            // texture space flips y axis just to piss you off :)
            uv_rect.top = remapFloat(0, (float) height, 0, 1, (float) node->rect.top);
            uv_rect.bottom = remapFloat(0, (float) height, 0, 1, (float) node->rect.bottom+1);
            textureMap[idMap[pb]] = std::unique_ptr<AtlasTexture>(new AtlasTexture(pb->getName(), node->rect, uv_rect, idMap[pb], pb->getWidth(), pb->getHeight()));
            //SDL_Log("Fitted image %s size: %dx%d in atlas", pb->getName().c_str(), pb->getWidth(), pb->getHeight());
        }
        else
        {
            SDL_Log("Could not fit image %s size: %dx%d", pb->getName().c_str(), pb->getWidth(), pb->getHeight());
            unfittedIds.push_back(idMap[pb]);
        }
    }
    built = true;
    //atlas->saveToPNG("atlas.png");
    return unfittedIds.empty();
}

PixelBuffer *TextureAtlas::getBuffer() {
    return atlas.get();
}

void TextureAtlas::debug()
{
    for(auto const& tex : textureMap)
    {
        auto t = tex.second.get();
        SDL_Log("[%d] Texture: %s px = (%d,%d,%d,%d) uv = (%.2f,%.2f,%.2f,%.2f)", tex.first, t->name.c_str(), t->rect.left, t->rect.top, t->rect.right, t->rect.bottom, t->uvRect.left, t->uvRect.top, t->uvRect.right, t->uvRect.bottom);
    }
}

void TextureAtlas::upload() {
    texture = std::unique_ptr<OGLTexture>(new OGLTexture(atlas.get()));
    SDL_Log("Uploaded %d atlas textures", textureMap.size());
}

UVRect& TextureAtlas::getUVRect(int32_t handle) {
    //debug();
    try {
        return textureMap[handle]->uvRect;
    }
    catch (std::out_of_range& e)
    {
        SDL_Log("std::out_of_range arg = %d", handle);
        abort();
    }
}

void TextureAtlas::bind() {
    if(texture != nullptr)
        texture->bind();
}

void TextureAtlas::bind(int tex_unit) {
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture->tex);
}

std::shared_ptr<OGLTexture> TextureAtlas::createOGLTexture(int32_t handle) {
    auto& tex = textureMap[handle];
    std::unique_ptr<PixelBuffer> pb = std::unique_ptr<PixelBuffer>(new PixelBuffer(static_cast<uint32_t>(tex->width), static_cast<uint32_t>(tex->height)));
    Rect2D r(tex->rect.left, tex->rect.top, tex->width, tex->height);
    pb->copy(atlas.get(), &r, 0, 0);
    return std::make_shared<OGLTexture>(pb.get(), true);
}


/*
 * bin packing. recursively subdivide squares and fit rectangles
 */
AtlasNode *AtlasNode::insert(PixelBuffer &pb) {
    //SDL_Log("Inserting PB: %dx%d", pb.getWidth(), pb.getHeight());
    AtlasNode *newNode = nullptr;
    // if we're not a leaf
    if(child[0] != nullptr && child[1] != nullptr)
    {
        // try insert into first node..
        newNode = child[0]->insert(pb);
        if(newNode != nullptr)
            return newNode;

        // no room, insert into second
        return child[1]->insert(pb);
    }
    // we're a leaf, watch us soar (no children)
    // if there is already an image here, return
    if(buffer != nullptr)
        return nullptr;
    // if we're to small, return
    if(rect.width() < pb.getWidth() || rect.height() < pb.getHeight())
    {
        return nullptr;
    }
    // if we fit exactly, return this
    if(rect.width() == pb.getWidth() && rect.height() == pb.getHeight())
        return this;

    // otherwise we split the node in two
    //SDL_Log("Splitting node");
    child[0] = std::unique_ptr<AtlasNode>(new AtlasNode());
    child[1] = std::unique_ptr<AtlasNode>(new AtlasNode());
    // decide which way to split
    auto dw = rect.width() - pb.getWidth();
    auto dh = rect.height() - pb.getHeight();
    // perform the split
    if(dw == 0)         // if fits perfectly vertically, split horizontally
    {
        child[0]->rect.set(rect.left, rect.top, rect.right, rect.top + pb.getHeight() - 1);
        child[1]->rect.set(rect.left, rect.top + pb.getHeight(), rect.right, rect.bottom);
    }
    else if(dh == 0)    // fits perfectly horizontally, split vertically
    {
        child[0]->rect.set(rect.left, rect.top, rect.left + pb.getWidth() - 1, rect.bottom);
        child[1]->rect.set(rect.left + pb.getWidth(), rect.top, rect.right, rect.bottom);
    }
    else if(dw < dh)    // split the way is the closest fit
    {
        child[0]->rect.set(rect.left, rect.top, rect.left + pb.getWidth() - 1, rect.bottom);
        child[1]->rect.set(rect.left + pb.getWidth(), rect.top, rect.right, rect.bottom);
    }
    else
    {
        child[0]->rect.set(rect.left, rect.top, rect.right, rect.top + pb.getHeight() - 1);
        child[1]->rect.set(rect.left, rect.top + pb.getHeight(), rect.right, rect.bottom);
    }
    // insert into first child created
    return child[0]->insert(pb);
}
