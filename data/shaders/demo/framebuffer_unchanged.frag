#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTex;

void main()
{
	FragColor = vec4(texture(screenTex, TexCoord).rgb, 1.0);
}
