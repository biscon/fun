//
// Created by bison on 13-08-2017.
//

#include <SDL.h>
#include "Shader.h"

Shader::Shader(const char *source, GLenum type) : source(source), type(type) {
    shaderId = glCreateShader(type);
}

void Shader::compile() {
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);
    GLint status = GL_FALSE;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        glGetShaderInfoLog(shaderId, 512, NULL, buffer);
        SDL_Log("Shader Error Log:\n%s", buffer);
    }
}

