#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ALBEDO;
uniform vec3 color = vec3(1.0);

void main()
{
	FragColor = texture(ALBEDO, TexCoords);
	FragColor.rgb *= color;
}