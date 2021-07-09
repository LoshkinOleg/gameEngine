#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aModelPos;

out vec2 TexCoords;
out float DistToEmitter;
out vec3 color;

uniform mat4 cameraMatrix;
uniform vec3 emitterPos;
uniform float scale;

void main()
{
	TexCoords = aTexCoord;
	color.r = tan(gl_InstanceID);
	color.g = tan(gl_InstanceID + 1.0);
	color.b = tan(gl_InstanceID + 2.0);

	mat4 modelMatrix = mat4(1.0);

	DistToEmitter = length(emitterPos - aModelPos);

	modelMatrix[0][0] = scale;
	modelMatrix[1][1] = scale;
	modelMatrix[2][2] = scale;

	modelMatrix[3][0] = aModelPos.x;
	modelMatrix[3][1] = aModelPos.y;
	modelMatrix[3][2] = aModelPos.z;

	gl_Position = cameraMatrix * modelMatrix * vec4(aPos, 1.0);
}