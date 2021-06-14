#version 440 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTex;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoord = aTex;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
