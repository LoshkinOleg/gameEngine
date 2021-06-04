#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord; // Needs to be in location 2 because of the way I load models: 0: pos, 1: normals, 2: texcoords

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}