#version 440 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ambient;

void main()
{
    FragColor = vec4(texture(ambient, TexCoord).rgb, 1.0);
}