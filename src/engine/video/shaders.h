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

// Lighting ------------------------------------------------------------------------------------------------------------

static const char* lightingShaderVertexSrc = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    out vec3 Normal;
    out vec3 FragPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        FragPos = vec3(model * vec4(aPos, 1.0));
        //Normal = aNormal;
        Normal = mat3(transpose(inverse(model))) * aNormal;
    }
)glsl";

static const char* lightingShaderFragmentSrc = R"glsl(
    #version 330 core

    out vec4 FragColor;

    in vec3 Normal;
    in vec3 FragPos;

    uniform vec3 viewPos;

    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    uniform Material material;

    struct Light {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    uniform Light light;

    void main()
    {
        // ambient
        vec3 ambient  = light.ambient * material.ambient;

        // diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse  = light.diffuse * (diff * material.diffuse);

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
)glsl";

static const char* lampShaderVertexSrc = R"glsl(
    #version 330 core

    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)glsl";

static const char* lampShaderFragmentSrc = R"glsl(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0); // set alle 4 vector values to 1.0
    }
)glsl";


#endif //GAME_SHADERS_H
