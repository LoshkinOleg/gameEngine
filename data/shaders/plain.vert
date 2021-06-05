#version 440 core

layout (location = 0) in vec3 aPos; // Note: Be careful with the number of attribs you declare in the shader vs. enable in the code! It must be the same!

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}