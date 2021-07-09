#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

struct Material
{
	sampler2D diffuseMap;
};

uniform Material material;

void main()
{
	FragColor = texture(material.diffuseMap, TexCoords);
}