#version 440 core

layout (location = 0) out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube cubemap;

void main()
{
	FragColor = texture(cubemap, TexCoord);
}