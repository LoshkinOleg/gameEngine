#version 440 core

layout (location = 0) out vec4 fbTexture0;
layout (location = 1) out vec4 fbTexture1;
layout (location = 2) out vec4 fbTexture2;
layout (location = 3) out vec4 fbTexture3;
layout (location = 4) out vec4 fbTexture4;
layout (location = 5) out vec4 fbTexture5;

in vec3 TexCoord;

uniform samplerCube cubemap;

void main()
{
	fbTexture0 = texture(cubemap, TexCoord);
	fbTexture1 = vec4(0.0);
	fbTexture2 = vec4(0.0);
	fbTexture3 = vec4(0.0);
	fbTexture4 = vec4(0.0);
	fbTexture5 = vec4(0.0);
}