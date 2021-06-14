#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord; // I'm always loading texcoords in location = 2 for onj imported models, location = 1 is for normals.

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}