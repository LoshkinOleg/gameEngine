#version 440 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;
out float MaxBrightness; 

uniform sampler2D fbTexture0;

const float maxBrightness = 1.0;

void main()
{
    MaxBrightness = maxBrightness;
	TexCoord = aPos * 0.5 + 0.5;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}