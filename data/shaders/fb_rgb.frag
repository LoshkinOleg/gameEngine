#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture;

void main()
{
	FragColor = vec4(texture(fbTexture, TexCoord).rgb, 1.0);
}