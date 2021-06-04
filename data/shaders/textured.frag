#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
    FragColor = vec4(texture(ambient, TexCoord).rgb, 1.0);
}