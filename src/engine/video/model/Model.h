//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <glm.hpp>
#include <memory>
#include <string>
#include <engine/asset/ILoadTask.h>
#include <set>
#include "engine/video/mesh/MeshUpdateType.h"
#include "engine/video/shader/Shader.h"
#include "engine/video/texture/OGLTexture.h"
#include "engine/video/mesh/Mesh2.h"
#include "MaterialDictionary.h"
#include "engine/video/mesh/Mesh3.h"

class Model : public ILoadTask {
public:
    Model(const std::string &filename, const std::string &basepath);

    bool loadOBJ();
    bool load(IGame &game) override;
    bool prepare(IGame &game) override;
    void draw(Shader& shader);

private:
    std::unique_ptr<MaterialDictionary> materialDictionary;
    std::vector<std::unique_ptr<Mesh3>> meshes;
    std::map<Mesh3*, std::string> meshMaterialMap;


    std::string filename;
    std::string basepath;
};


#endif //GAME_MODEL_H
