//
// Created by bison on 20-11-2017.
//

#ifndef GAME_TEXTUREATLAS_H
#define GAME_TEXTUREATLAS_H

#include <utility>
#include <vector>
#include <memory>
#include <map>
#include "PixelBuffer.h"
#include "OGLTexture.h"

struct AtlasRect {
public:
    AtlasRect() {}
    AtlasRect(i32 left, i32 top, i32 right, i32 bottom) : left(left), top(top), right(right),
                                                                          bottom(bottom) {}
    void set(i32 left, i32 top, i32 right, i32 bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    i32 width() {
        return (right - left) + 1;
    }

    i32 height() {
        return (bottom - top) + 1;
    }

    i32 left,top,right,bottom;
};

/*
 * Nodes own their children, when root node dies with texture atlas all debts will be
 * forgotten and all slaves freed, muaaaaaaah
 */
struct AtlasNode
{
    AtlasNode* insert(PixelBuffer &pb);
    std::unique_ptr<AtlasNode> child[2] = {nullptr, nullptr};
    AtlasRect rect;
    std::shared_ptr<PixelBuffer> buffer = nullptr;
};

struct UVRect
{
    float top=0,left=0,bottom=0,right=0;
};

struct AtlasTexture
{
    AtlasTexture(std::string name, const AtlasRect &rect, const UVRect &uvRect, i32 id, i32 width, i32 height) : name(std::move(name)), rect(rect),
                                                                                         uvRect(uvRect), id(id), width(width), height(height) {}
    std::string name;
    AtlasRect rect;
    UVRect uvRect;
    i32 id;
    i32 width;
    i32 height;
};

class TextureAtlas {
public:
    TextureAtlas(u32 width, u32 height, bool filtering);

    i32 addBuffer(std::shared_ptr<PixelBuffer> pb);

    bool build();
    void upload();

    PixelBuffer *getBuffer();

    void debug();

    bool isBuilt() const {
        return built;
    }

    const std::vector<i32> &getUnfittedIds() const {
        return unfittedIds;
    }

    UVRect& getUVRect(i32 handle);
    void bind();
    void bind(int tex_unit);
    std::shared_ptr<OGLTexture> createOGLTexture(i32 handle);
    std::shared_ptr<OGLTexture> getTexture();

private:
    bool built = false;
    u32 width, height;
    std::unique_ptr<AtlasNode> root = nullptr;
    std::unique_ptr<PixelBuffer> atlas;
    std::vector<std::shared_ptr<PixelBuffer>> buffers;
    std::map<std::shared_ptr<PixelBuffer>, i32> idMap;
    std::map<i32, std::unique_ptr<AtlasTexture>> textureMap;
    std::vector<i32> unfittedIds;
    std::shared_ptr<OGLTexture> texture;
    i32 nextHandle = 1;
    bool useFiltering;
};


#endif //GAME_TEXTUREATLAS_H
