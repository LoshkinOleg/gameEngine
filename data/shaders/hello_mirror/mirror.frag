#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTex;

vec4 Default();

void main()
{
	FragColor = Default();
}

vec4 Default()
{
	return vec4(texture(screenTex, TexCoord).rgb, 1.0);
}