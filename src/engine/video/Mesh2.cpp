//
// Created by bison on 16-12-2017.
//

#include "Mesh2.h"

Mesh2::Mesh2(std::vector<Vertex> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices) {

}

Mesh2::Mesh2(std::vector<Vertex> vertices) : vertices(vertices) {

}

void Mesh2::prepare()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if(!indices.empty())
        glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    if(!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh2::draw(const Shader& shader)
{
    if(material->diffuseTexture != nullptr)
    {
        glActiveTexture(GL_TEXTURE0);
        shader.setInt("material.texture_diffuse1", 0);
        glBindTexture(GL_TEXTURE_2D, material->diffuseTexture->tex);
    }
    if(material->specularTexture != nullptr)
    {
        glActiveTexture(GL_TEXTURE1);
        shader.setInt("material.texture_specular1", 1);
        glBindTexture(GL_TEXTURE_2D, material->specularTexture->tex);
    }
    glActiveTexture(GL_TEXTURE0);

    shader.setFloat("material.shininess", material->shininess);


    // draw mesh
    glBindVertexArray(VAO);
    if(!indices.empty())
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

