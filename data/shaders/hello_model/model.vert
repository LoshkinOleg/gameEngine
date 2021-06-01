#version 440 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTex;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vs_out.TexCoord = aTex;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}
