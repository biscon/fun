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
#include "engine/video/shader/Shader.h"
#include "engine/video/texture/OGLTexture.h"
#include "engine/video/shader/Material.h"
#include "engine/video/texture/TextureAtlas.h"

// make use indexed rendering

class Mesh3 {
public:
    Mesh3();
    explicit Mesh3(std::vector<float> vertices);
    Mesh3(std::vector<float> vertices, std::vector<unsigned int> indices);

    virtual ~Mesh3();

    void draw(const Shader& shader);
    void drawRange(const Shader& shader, i32 start, i32 count, Material* material);
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
    u32 vertexSize = 6;
    bool hasNormals = true;
    bool hasTexcoords = false;
    MeshUpdateType type = MeshUpdateType::STATIC;

    inline size_t getMeshSizeBytes()
    {
        return vertices.size() * sizeof(float);
    }

private:
    void init();
    /*  Render data  */
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    /*  Functions    */
};


#endif //GAME_MESH3_H
