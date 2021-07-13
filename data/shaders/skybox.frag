#version 440 core

layout (location = 0) out vec4 fbTexture0; // rgb: albedo, w: shininess
layout (location = 1) out vec4 fbTexture1; // xyz: w_FragPos
layout (location = 2) out vec4 fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
layout (location = 3) out vec4 fbTexture3; // x: shininess
// layout (location = 4) out vec4 fbTexture4;
// layout (location = 5) out vec4 fbTexture5;

in vec3 TexCoord;

uniform samplerCube cubemap;

const float INVALID_FLOAT = 1.0/0.0; // Allowed by glsl. Can be checked via isinf().

void main()
{
    fbTexture0 = vec4(1.0);
    fbTexture1 = vec4(1.0);
    fbTexture2 = vec4(1.0);
    fbTexture3 = vec4(1.0);

	fbTexture0 = vec4(pow(texture(cubemap, TexCoord).rgb, vec3(1.0/2.2)), 1.0); // TODO: figure out how to specify that GLI should load textures as sRGB...
	fbTexture1.xyz = vec3(INVALID_FLOAT); // Don't render skybox using blinn-phong.
	fbTexture2.xyz = vec3(INVALID_FLOAT);
	fbTexture3.xyz = vec3(INVALID_FLOAT); // No shininess.
}