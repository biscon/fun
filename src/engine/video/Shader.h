//
// Created by bison on 13-08-2017.
//
#ifndef GAME_GLSLSHADER_H
#define GAME_GLSLSHADER_H

#define GLEW_STATIC
#include <GL/glew.h>

class Shader {
public:
    explicit Shader(const char *source, GLenum type);
    void compile();

    GLuint getShaderId() const {
        return shaderId;
    }

private:
    const char *source;
    GLenum type;
    GLuint shaderId;
};


#endif //GAME_GLSLSHADER_H
