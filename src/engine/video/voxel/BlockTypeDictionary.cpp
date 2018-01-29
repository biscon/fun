//
// Created by bison on 26-12-2017.
//

#include <algorithm>
#include <engine/util/string_util.h>
#include "BlockTypeDictionary.h"

std::string material_idx_color = "material[i].color";
std::string material_idx_texture = "material[i].texture";
std::string material_idx_has_texture = "material[i].hasTexture";
std::string material_idx_layer = "material[i].layer";

void BlockType::applyShader(const Shader& shader)
{
    for(i32 i = 0; i < 6; i++)
    {
        switch(i)
        {
            case 0: {
                material_idx_color[9] = '0';
                material_idx_texture[9] = '0';
                material_idx_has_texture[9] = '0';
                material_idx_layer[9] = '0';
                break;
            }
            case 1: {
                material_idx_color[9] = '1';
                material_idx_texture[9] = '1';
                material_idx_has_texture[9] = '1';
                material_idx_layer[9] = '1';
                break;
            }
            case 2: {
                material_idx_color[9] = '2';
                material_idx_texture[9] = '2';
                material_idx_has_texture[9] = '2';
                material_idx_layer[9] = '2';
                break;
            }
            case 3: {
                material_idx_color[9] = '3';
                material_idx_texture[9] = '3';
                material_idx_has_texture[9] = '3';
                material_idx_layer[9] = '3';
                break;
            }
            case 4: {
                material_idx_color[9] = '4';
                material_idx_texture[9] = '4';
                material_idx_has_texture[9] = '4';
                material_idx_layer[9] = '4';
                break;
            }
            case 5: {
                material_idx_color[9] = '5';
                material_idx_texture[9] = '5';
                material_idx_has_texture[9] = '5';
                material_idx_layer[9] = '5';
                break;
            }
        }
        shader.setVec4(material_idx_color, material[i]->color);
        shader.setInt(material_idx_layer, material[i]->layer);

        if(material[i]->layer != -1) {
            //glActiveTexture(GL_TEXTURE0);
            shader.setInt(material_idx_texture, 0);
            //texture->bind();
            shader.setInt(material_idx_has_texture, 1);
        }
        else
            shader.setInt(material_idx_has_texture, 0);
    }
}

BlockTypeDictionary::BlockTypeDictionary() {
    arrayTexture = std::unique_ptr<OGLArrayTexture>(new OGLArrayTexture());
}

bool BlockTypeDictionary::load(IGame &game) {
    for(auto const& mat : materials)
    {
        // textured material
        if(mat->layer != -1) {
            mat->buffer = std::make_shared<PixelBuffer>(mat->textureMap);
        }
    }
    return true;
}

bool BlockTypeDictionary::prepare(IGame &game) {
    auto sum = 0;
    for(auto const& mat : materials)
        if(mat->buffer != nullptr)
            sum++;
    arrayTexture->init(faceTextureSize, faceTextureSize, sum);
    for(auto const& mat : materials)
    {
        if(mat->layer != -1) {
            arrayTexture->uploadLayer(mat->buffer.get(), mat->layer);
        }
    }
    arrayTexture->finalizeUpload();
    return true;
}

FaceMaterial* BlockTypeDictionary::addTexturedMaterialIfNotPresent(std::string filename)
{
    for(auto &mat : materials)
    {
        if(mat->textureMap == filename)
            return mat.get();
    }
    materials.push_back(std::unique_ptr<FaceMaterial>(new FaceMaterial()));
    auto &mat = materials.back();
    mat->textureMap = filename;
    mat->color = glm::vec4(0,0,0,0);
    mat->layer = curLayer++;

    SDL_Log("Added textured material %s", filename.c_str());
    return mat.get();
}

FaceMaterial *BlockTypeDictionary::addColoredMaterialIfNotPresent(glm::vec4 color) {
    for(auto &mat : materials)
    {
        if(mat->color == color)
            return mat.get();
    }
    materials.push_back(std::unique_ptr<FaceMaterial>(new FaceMaterial()));
    auto &mat = materials.back();
    mat->textureMap = "";
    mat->color = color;
    mat->layer = -1;

    SDL_Log("Added colored material");
    return mat.get();
}

void BlockTypeDictionary::createBlockType(std::string name, std::string diffuseMapFilename) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->texture = arrayTexture.get();
    // use the same texture for all faces
    for(i32 i = 0; i < 6; i++)
    {
        bt->material[i] = addTexturedMaterialIfNotPresent(diffuseMapFilename);
    }
}

BlockType &BlockTypeDictionary::getBlockTypeAt(int index) {
    return *blockTypes[index];
}

void BlockTypeDictionary::createBlockType(std::string name, glm::vec4 diffuse) {
    blockTypes.push_back(std::unique_ptr<BlockType>(new BlockType()));
    auto& bt = blockTypes.back();
    bt->name = name;
    bt->texture = nullptr;
    // use the same color for all faces
    for(i32 i = 0; i < 6; i++)
    {
        bt->material[i] = addColoredMaterialIfNotPresent(diffuse);
    }
}

