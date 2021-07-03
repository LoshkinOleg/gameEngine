#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out vec2 TexCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	TexCoords = aTexCoord;
	gl_Position = projectionMatrix * mat4(mat3(viewMatrix)) * aModel * vec4(aPos, 1.0);
}