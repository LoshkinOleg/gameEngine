#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture;

// Add gamma correction + exposure hdr

void main()
{
	const vec3 color = texture(fbTexture, TexCoord).rgb;
	FragColor = vec4(color / (color + vec3(1.0)), 1.0);
}