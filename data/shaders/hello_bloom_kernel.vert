#version 440 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;

void main()
{
	// From [-1;1] to [0,1] range.
	TexCoord = aPos * 0.5 + 0.5;

	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}