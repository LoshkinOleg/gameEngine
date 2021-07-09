#version 440 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in float DistToEmitter;
in vec3 color;

struct Material
{
	sampler2D alphaMap;
};

uniform Material material;
uniform float emitterRadius;

void main()
{
	const float alpha = texture(material.alphaMap, TexCoords).r;
	if (alpha < 1.0) discard;
	FragColor.rgb = color;
	FragColor.a = alpha * (emitterRadius - DistToEmitter);
}