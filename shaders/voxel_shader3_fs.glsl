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
uniform float sunlightIntensity;

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
    float sun = sunlightIntensity * (VertexColor.x / 15.0);
    float torch = VertexColor.y / 15.0;

    vec3 sunlight = vec3(sun, sun, sun);
    vec3 torchlight = vec3(torch, 0.95 * torch, 0.90 * torch);

    float ao = VertexColor.z / 3.0;
    float ao_factor = 0.75 + ao * (1.0 - 0.75) / 1.0;

    vec3 light = clamp(sunlight + torchlight, 0.05, 1.0);

    vec3 final_color = mix(vec3(0,0,0), light, ao_factor);

    if(material.hasTexture == 1)
    {
        //result = vec4(VertexColor / 256.0, 1.0) * texture(texture_diffuse, TexCoord);
        result = vec4(final_color, 1.0) * texture(texture_diffuse, TexCoord);
    }
    else
    {
        result = vec4(final_color, 1.0) * material.diffuse;
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