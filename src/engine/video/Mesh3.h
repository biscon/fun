//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MESH3_H
#define GAME_MESH3_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <glm.hpp>
#include <memory>
#include <string>
#include "MeshUpdateType.h"
#include "Shader.h"
#include "OGLTexture.h"
#include "Material.h"
#include "TextureAtlas.h"

class Mesh3 {
public:
    Mesh3();
    explicit Mesh3(std::vector<float> vertices);
    Mesh3(std::vector<float> vertices, std::vector<unsigned int> indices);

    virtual ~Mesh3();

    void draw(const Shader& shader);
    void drawRange(const Shader& shader, int32_t start, int32_t count, Material* material);
    void prepare();
    void upload();
    void generateCubeAt(float x, float y, float z);
    void generateTexturedCubeAt(float x, float y, float z, UVRect& r);

    void clear();


    /*  Mesh Data  */
    std::vector<float> vertices;
    //std::vector<float> cube;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;

    // floats per vertex
    uint32_t vertexSize = 6;
    bool hasNormals = true;
    bool hasTexcoords = false;
    MeshUpdateType type = MeshUpdateType::STATIC;

private:
    void init();
    /*  Render data  */
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    /*  Functions    */
};


#endif //GAME_MESH3_H
