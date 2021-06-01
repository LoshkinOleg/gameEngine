#version 440 core

in vec3 aPos;

uniform mat4 perspective;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}