#version 440 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	TexCoord = aPos;
	gl_Position = (projectionMatrix * mat4(mat3(viewMatrix)) * vec4(aPos, 1.0)).xyww; // w = 1.0, therefore during the perspective division, z will always be set to w / w = 1.0.
}