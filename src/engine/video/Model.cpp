//
// Created by bison on 16-12-2017.
//

#include "Model.h"

//#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <engine/asset/tiny_obj_loader.h>
#include <iostream>

Model::Model(const std::string &filename, const std::string &basepath) : filename(filename), basepath(basepath) {
    materialDictionary = std::unique_ptr<MaterialDictionary>(new MaterialDictionary());
}

bool Model::loadOBJ() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), basepath.c_str(), true);

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        return false;
    }

    std::vector<Vertex> vertices;
    //std::vector<std::shared_ptr<OGLTexture>> textures;

// Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {

        // create mesh
        vertices.clear();

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = 1.0f - attrib.texcoords[2*idx.texcoord_index+1];

                Vertex vertex{};
                vertex.Position = glm::vec3(vx, vy, vz);
                vertex.Normal = glm::vec3(nx, ny, nz);
                vertex.TexCoords = glm::vec2(tx, ty);
                vertices.push_back(vertex);

                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
            }
            index_offset += fv;

            // per-face material
            //auto mat_id = shapes[s].mesh.material_ids[f];
        }
        meshes.push_back(std::unique_ptr<Mesh2>(new Mesh2(vertices)));

        std::vector<TextureBufferItem*> mesh_textures;
        auto mat_id = shapes[s].mesh.material_ids[0];
        auto mat = materials.at(mat_id);
        SDL_Log("Processing material %s", mat.name.c_str());

        materialDictionary->createMaterial(mat.name,
                                           glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]),
                                           glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]),
                                           glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]),
                                           mat.diffuse_texname, mat.specular_texname, mat.shininess);

        SDL_Log("Building submesh %s containing %d vertices", shapes[s].name.c_str(), vertices.size());
        meshMaterialMap[meshes.back().get()] = mat.name;
    }
    return true;
}

bool Model::load(IGame &game) {
    SDL_Log("Loading model %s", filename.c_str());
    loadOBJ();
    materialDictionary->load(basepath);
    return true;
}

bool Model::prepare(IGame &game) {
    SDL_Log("Preparing model %s", filename.c_str());
    // upload textures
    materialDictionary->prepare();
    SDL_Log("Uploading %d meshes", meshes.size());
    for(auto& mesh : meshes)
    {
        mesh->material = materialDictionary->getMaterial(meshMaterialMap[mesh.get()]);
        // upload mesh
        mesh->prepare();
    }
    return true;
}

void Model::draw(Shader &shader) {
    for(auto& mesh : meshes)
    {
        mesh->draw(shader);
    }
}

