//
// Created by bison on 16-12-2017.
//

#ifndef GAME_MESH2_H
#define GAME_MESH2_H

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

struct Vertex {
    Vertex() {}
    Vertex(const glm::vec3 &Position, const glm::vec3 &Normal, const glm::vec2 &TexCoords) : Position(Position),
                                                                                             Normal(Normal), TexCoords(TexCoords) {}
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh2 {
public:
    /*  Functions  */
    explicit Mesh2(std::vector<Vertex> vertices);
    Mesh2(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void draw(const Shader& shader);
    void prepare();

    void addTexturedCubeWithNormals(float x, float y, float z);

    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;

private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
};


#endif //GAME_MESH2_H
