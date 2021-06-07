#version 440 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D screenTex;

void main()
{
	if (FragPos.x > 0) // apply tone mapping
	{
		vec3 hdrColor = texture(screenTex, TexCoord).rgb;
		vec3 mapped = hdrColor / (hdrColor + vec3(1.0)); // reinhard tone mapping
		FragColor = vec4(mapped, 1.0);
	}
	else // Display as is.
	{
		FragColor = vec4(texture(screenTex, TexCoord).rgb, 1.0);
	}
}