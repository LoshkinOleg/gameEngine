#version 440 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoord = aPos;
	gl_Position = (projection * mat4(mat3(view)) * vec4(aPos, 1.0)).xyww; // w = 1.0, therefore during the perspective division, z will always be set to w / w = 1.0.
}