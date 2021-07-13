#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aModelPos;

out vec2 TexCoords;
out vec3 color;

uniform mat4 cameraMatrix;

const float SCALE = 0.05;

void main()
{
	TexCoords = aTexCoord;
	color.r = clamp(tan(gl_InstanceID), 0.0, 1.0);
	color.g = clamp(tan(gl_InstanceID + 1.0), 0.0, 1.0);
	color.b = clamp(tan(gl_InstanceID + 2.0), 0.0, 1.0);

	mat4 modelMatrix = mat4(1.0);

	modelMatrix[0][0] = SCALE;
	modelMatrix[1][1] = SCALE;
	modelMatrix[2][2] = SCALE;

	modelMatrix[3][0] = aModelPos.x;
	modelMatrix[3][1] = aModelPos.y;
	modelMatrix[3][2] = aModelPos.z;

	gl_Position = cameraMatrix * modelMatrix * vec4(aPos, 1.0);
}