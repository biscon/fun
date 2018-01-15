#version 330 core
out vec4 FragColor;

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    int hasTexture;
    float shininess;
};

struct Fog
{
    int is_active;
    vec3 color;
    float density;
};

in vec3 VertexPos;
in vec3 VertexNormal;
in vec3 VertexColor;
in vec2 TexCoord;

uniform vec3 camPos;
uniform vec3 ambientLight;
uniform Material material;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform Fog fog;

vec4 calcFog(vec3 pos, vec4 color, Fog fog)
{
    vec3 fogColor = fog.color;

    float distance = length(pos);
    float fogFactor = 1.0 / exp( (distance * fog.density)* (distance * fog.density));
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 resultColor = mix(fogColor, color.xyz, fogFactor);
    return vec4(resultColor.xyz, color.w);
}

void main()
{
    // properties
    //vec3 norm = normalize(VertexNormal);
    vec4 result;
    //result = texture(texture_diffuse, TexCoord);

    if(material.hasTexture == 1)
    {
        //result = vec4(VertexColor / 256.0, 1.0) * texture(texture_diffuse, TexCoord);
        result = vec4(VertexColor.x / 256.0, VertexColor.y / 256.0, VertexColor.z / 256.0, 1.0) * texture(texture_diffuse, TexCoord);
    }
    else
    {
        result = vec4(VertexColor / 256.0, 1.0) * material.diffuse;
    }

    FragColor = result;

    if(fog.is_active == 1)
    {
        FragColor = calcFog(camPos - VertexPos, FragColor, fog);
    }

    // apply gamma correction
    //float gamma = 2.2;
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}