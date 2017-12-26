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
    AtlasRect(int32_t left, int32_t top, int32_t right, int32_t bottom) : left(left), top(top), right(right),
                                                                          bottom(bottom) {}
    void set(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    int32_t width() {
        return (right - left) + 1;
    }

    int32_t height() {
        return (bottom - top) + 1;
    }

    int32 left,top,right,bottom;
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
    AtlasTexture(std::string name, const AtlasRect &rect, const UVRect &uvRect, int32_t id, int32_t width, int32_t height) : name(std::move(name)), rect(rect),
                                                                                         uvRect(uvRect), id(id), width(width), height(height) {}
    std::string name;
    AtlasRect rect;
    UVRect uvRect;
    int32_t id;
    int32_t width;
    int32_t height;
};

class TextureAtlas {
public:
    TextureAtlas(uint32_t width, uint32_t height, bool filtering);

    int32_t addBuffer(std::shared_ptr<PixelBuffer> pb);

    bool build();
    void upload();

    PixelBuffer *getBuffer();

    void debug();

    bool isBuilt() const {
        return built;
    }

    const std::vector<int32_t> &getUnfittedIds() const {
        return unfittedIds;
    }

    UVRect& getUVRect(int32_t handle);
    void bind();
    void bind(int tex_unit);
    std::shared_ptr<OGLTexture> createOGLTexture(int32_t handle);
    std::shared_ptr<OGLTexture> getTexture();

private:
    bool built = false;
    uint32_t width, height;
    std::unique_ptr<AtlasNode> root = nullptr;
    std::unique_ptr<PixelBuffer> atlas;
    std::vector<std::shared_ptr<PixelBuffer>> buffers;
    std::map<std::shared_ptr<PixelBuffer>, int32_t> idMap;
    std::map<int32_t, std::unique_ptr<AtlasTexture>> textureMap;
    std::vector<int32_t> unfittedIds;
    std::shared_ptr<OGLTexture> texture;
    int32_t nextHandle = 1;
    bool useFiltering;
};


#endif //GAME_TEXTUREATLAS_H
