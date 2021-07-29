#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ALBEDO;

void main()
{
	const float color = texture(ALBEDO, TexCoords).r;
	FragColor = vec4(color, color, color, 1.0);
}