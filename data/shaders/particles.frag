#version 440 core

layout (location = 0) out vec4 fbTexture0; // rgb: albedo, w: shininess
layout (location = 1) out vec4 fbTexture1; // xyz: w_FragPos
layout (location = 2) out vec4 fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
layout (location = 3) out vec4 fbTexture3; // x: shininess

in vec2 TexCoords;
in vec3 color;

struct Material
{
	sampler2D alphaMap;
};

uniform Material material;

const float INVALID_FLOAT = 1.0/0.0; // Allowed by glsl. Can be checked via isinf().

void main()
{
	const float alpha = texture(material.alphaMap, TexCoords).r;

	// Draw particle only where it's not masked by alphamap. alpha is either 0.0 or 1.0
	if (alpha < 1.0)
	{
		discard;
	}
	else
	{
	    fbTexture0 = vec4(1.0);
		fbTexture1 = vec4(1.0);
		fbTexture2 = vec4(1.0);
		fbTexture3 = vec4(1.0);

		fbTexture0.rgb = color;

		fbTexture1.xyz = vec3(INVALID_FLOAT); // Don't apply blinn-phong to particles.
		fbTexture2.xyz = vec3(INVALID_FLOAT); // Nor normalmapping.
		fbTexture3.xyz = vec3(INVALID_FLOAT); // No shininess for particles.
	}
}