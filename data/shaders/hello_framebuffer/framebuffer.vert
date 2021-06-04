#version 440 core
in vec2 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 Pos;

void main()
{
	TexCoord = aTexCoord;
	Pos = aPos;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}