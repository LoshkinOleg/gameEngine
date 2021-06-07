#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTex;

void main()
{
	const float GAMMA = 2.2;
	vec3 hdrColor = texture(screenTex, TexCoord).rgb;
	hdrColor = pow(hdrColor, vec3(GAMMA)); // convert to linear color space.
	vec3 mapped = hdrColor / (hdrColor + vec3(1.0)); // reinhard tone mapping
	FragColor = vec4(pow(mapped, vec3(1.0 / GAMMA)), 1.0); // convert to sRGB
}