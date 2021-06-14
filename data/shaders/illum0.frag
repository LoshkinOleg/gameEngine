#version 440 core

layout (location = 0) out vec4 FragColor;

struct Material
{
    vec3 ambientColor;
};

uniform Material material;

void main()
{
    FragColor = vec4(material.ambientColor, 1.0);
}