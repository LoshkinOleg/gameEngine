#version 440 core

layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 PROJECTION;
uniform mat4 view;

void main()
{
	TexCoords = aPos * 0.5 + 0.5;
	gl_Position = PROJECTION * view * model * vec4(aPos, 0.0, 1.0);
}