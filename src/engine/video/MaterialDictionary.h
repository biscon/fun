//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MATERIALDICTIONARY_H
#define GAME_MATERIALDICTIONARY_H


#include <memory>
#include <map>
#include <string>
#include "OGLTexture.h"
#include "Material.h"

struct TextureBufferItem
{
    TextureBufferItem(const std::string &name, const std::string &type) : name(name), type(type) {}
    std::string name;
    std::string type;
    std::shared_ptr<PixelBuffer> buffer;
    std::shared_ptr<OGLTexture> texture;
};

class MaterialDictionary {
public:
    void createMaterial(std::string name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, std::string diffuseMap, std::string specularMap, float shininess);
    bool load(std::string basepath);
    bool prepare();
    std::shared_ptr<Material> getMaterial(const std::string& name);


private:
    std::map<std::string, std::shared_ptr<Material>> materialsMap;
    std::map<std::string, std::shared_ptr<OGLTexture>> texturesMap;
    std::map<std::string, std::unique_ptr<TextureBufferItem>> textureBufferItems;
};


#endif //GAME_MATERIALDICTIONARY_H
