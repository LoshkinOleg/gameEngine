#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture;

// Add gamma correction + exposure hdr

vec3 Reinhard(vec3 color);
vec3 ExtendedReinhard(vec3 color, float maxWhite);

void main()
{
	const vec3 color = texture(fbTexture, TexCoord).rgb;
	FragColor = vec4(ExtendedReinhard(color, 1.0), 1.0);
}

vec3 Reinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 ExtendedReinhard(vec3 color, float maxWhite)
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}