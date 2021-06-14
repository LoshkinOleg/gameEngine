#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

struct Material
{
    sampler2D ambientMap;
    vec3 ambientColor;
};

uniform Material material;

void main()
{
    FragColor = vec4(texture(material.ambientMap, TexCoord).rgb * material.ambientColor, 1.0);
}