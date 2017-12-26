//
// Created by bison on 25-12-2017.
//

#include "TileRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SDL_log.h>

TileRenderer::TileRenderer(ISystem &system, const std::shared_ptr<Camera> &camera) : system(system), camera(camera) {
    auto lighting_vs = system.readTextFile("shaders/material_lighting_shader_vs.glsl");
    auto lighting_fs = system.readTextFile("shaders/material_lighting_shader_fs.glsl");

    //auto lighting_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");

    shader = std::unique_ptr<Shader>(new Shader(lighting_vs.c_str(), lighting_fs.c_str(), nullptr));

    auto lamp_vs = system.readTextFile("shaders/lamp_shader_vs.glsl");
    auto lamp_fs = system.readTextFile("shaders/lamp_shader_fs.glsl");
    lampShader = std::unique_ptr<Shader>(new Shader(lamp_vs.c_str(), lamp_fs.c_str(), nullptr));
}

void TileRenderer::render(float screenWidth, float screenHeight, double time) {

}
