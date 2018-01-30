//
// Created by bison on 10-12-2017.
//

#ifndef GAME_SHADER2_H
#define GAME_SHADER2_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
#include <glm.hpp>
#include <SDL_log.h>
#include <defs.h>

class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc, const char* geometrySrc = nullptr);
    // activate the shader
    // ------------------------------------------------------------------------
    __forceinline void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    __forceinline void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    __forceinline void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    __forceinline void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    __forceinline void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    __forceinline void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    __forceinline void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    __forceinline void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    __forceinline void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    __forceinline void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    __forceinline void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    __forceinline void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    __forceinline void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    __forceinline i32 getUniformLocation(const std::string &name)
    {
        return glGetUniformLocation(ID, name.c_str());
    }

    GLint getAttributePos(const std::string &name)
    {
        GLint pos = glGetAttribLocation(ID, name.c_str());
        if(pos < 0)
            SDL_Log("position attribute %s not found", name.c_str());
        return pos;
    }

    __forceinline GLuint getShaderId()
    {
        return ID;
    }

private:
    GLuint ID;
    void checkCompileErrors(GLuint shader, std::string type);
};


#endif //GAME_SHADER2_H
