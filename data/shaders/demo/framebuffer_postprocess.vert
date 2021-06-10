#version 440 core
in vec3 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Pos;

void main()
{
	TexCoord = aTexCoord;
	Pos = aPos;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}