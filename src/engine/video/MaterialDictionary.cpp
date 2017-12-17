//
// Created by bison on 16-12-2017.
//

#include <SDL_log.h>
#include "MaterialDictionary.h"

void MaterialDictionary::createMaterial(std::string name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                                        std::string diffuseMap, std::string specularMap, float shininess) {
    auto material = std::make_shared<Material>();
    material->name = name;
    material->ambient = ambient;
    material->diffuse = diffuse;
    material->specular = specular;
    material->shininess = shininess;
    material->diffuseMap = diffuseMap;
    material->specularMap = specularMap;
    materialsMap[name] = material;
    if(textureBufferItems.count(diffuseMap) == 0)
    {
        textureBufferItems[diffuseMap] = std::unique_ptr<TextureBufferItem>(new TextureBufferItem(diffuseMap, "texture_diffuse"));
    }
    if(textureBufferItems.count(specularMap) == 0)
    {
        textureBufferItems[specularMap] = std::unique_ptr<TextureBufferItem>(new TextureBufferItem(specularMap, "texture_specular"));
    }
}

bool MaterialDictionary::load(std::string basepath) {
    for(auto& kv : textureBufferItems)
    {
        SDL_Log("Loading texture map %s type %s", kv.first.c_str(), kv.second->type.c_str());
        kv.second->buffer = std::make_shared<PixelBuffer>(basepath + kv.first);
    }
    return true;
}

bool MaterialDictionary::prepare() {
    SDL_Log("Uploading %d texture maps", textureBufferItems.size());
    for(auto& kv : textureBufferItems) {
        kv.second->texture = std::make_shared<OGLTexture>(kv.second->buffer.get(), kv.second->type, true);
    }
    for(auto& kv : materialsMap)
    {
        if(textureBufferItems.count(kv.second->diffuseMap) == 1)
        {
            kv.second->diffuseTexture = textureBufferItems[kv.second->diffuseMap]->texture;
        }
        if(textureBufferItems.count(kv.second->specularMap) == 1)
        {
            kv.second->specularTexture = textureBufferItems[kv.second->specularMap]->texture;
        }
    }
    textureBufferItems.clear();
    return true;
}

std::shared_ptr<Material> MaterialDictionary::getMaterial(const std::string& name) {
    if(materialsMap.count(name) == 1)
        return materialsMap[name];
    return nullptr;
}
