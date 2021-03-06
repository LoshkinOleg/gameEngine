#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 cameraMatrix;

void main()
{
	FragPos = (aModel * vec4(aPos, 1.0)).xyz;
	Normal = transpose(inverse(mat3(aModel))) * aPos;
	gl_Position = cameraMatrix * aModel * vec4(aPos, 1.0);
}