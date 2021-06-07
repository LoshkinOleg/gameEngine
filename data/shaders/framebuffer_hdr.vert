#version 440 core
in vec3 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}