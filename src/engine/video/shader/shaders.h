//
// Created by bison on 19-11-2017.
//

#ifndef GAME_SHADERS_H
#define GAME_SHADERS_H

static const char* vertexSource2D = R"glsl(
    #version 330 core

    // read from vertex buffer
    in vec2 position;
    in vec4 color;
    in vec2 texcoord;

    // pass to fragment shader
    out vec4 Color;
    out vec2 Texcoord;

    // uniforms
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;

    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
    }
)glsl";

static const char* fragmentSource2D = R"glsl(
    #version 330 core

    // passed from vertex shader
    in vec4 Color;
    in vec2 Texcoord;

    // resulting fragment color
    out vec4 outColor;

    // texture
    uniform sampler2D tex;

    void main()
    {
        outColor = texture(tex, Texcoord) * Color;
        //outColor = vec4(Color, 1.0);
    }
)glsl";

static const char* vertexSource3D = R"glsl(
    #version 330 core

    // read from vertex buffer
    in vec3 position;
    in vec2 texcoord;

    // pass to fragment shader
    out vec2 Texcoord;

    // uniforms
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;

    void main()
    {
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";

static const char* fragmentSource3D = R"glsl(
    #version 330 core

    // passed from vertex shader
    in vec2 Texcoord;

    // resulting fragment color
    out vec4 outColor;

    // texture
    uniform sampler2D tex;

    void main()
    {
        outColor = texture(tex, Texcoord);
    }
)glsl";

#endif //GAME_SHADERS_H
