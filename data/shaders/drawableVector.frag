#version 440 core

layout (location = 0) out vec4 FragColor;

uniform vec3 COLOR;

void main()
{
	FragColor = vec4(COLOR, 1.0);
}