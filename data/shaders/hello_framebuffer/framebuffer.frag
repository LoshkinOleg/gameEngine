#version 440 core
out vec4 FragColor;

in vec3 Pos;
in vec2 TexCoord;

uniform sampler2D screenTex;

vec4 Invert();
vec4 Default();
vec4 Grayscale();
vec4 CorrectedGrayscale();
vec4 Sharpen();
vec4 Blur();
vec4 Edge();

void main()
{
	if (Pos.x > 0 && Pos.y > 0)
	{
		FragColor = Blur();
	}
	else if(Pos.x > 0 && Pos.y <= 0)
	{
		FragColor = Edge();
	}
	else if(Pos.x <= 0 && Pos.y <= 0)
	{
		FragColor = CorrectedGrayscale();
	}
	else
	{
		FragColor = Invert();
	}
}

vec4 Default()
{
	return vec4(texture(screenTex, TexCoord).rgb, 1.0);
}

vec4 Invert()
{
	return vec4(vec3(1.0 - texture(screenTex, TexCoord)), 0.0);
}

vec4 Grayscale()
{
	vec4 color = texture(screenTex, TexCoord);
	float avg = (color.r + color.g + color.b) / 3.0;
	return vec4(avg, avg, avg, 1.0);
}

vec4 CorrectedGrayscale()
{
	vec4 color = texture(screenTex, TexCoord);
	float avg = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
	return vec4(avg, avg, avg, 1.0);
}

vec4 Sharpen()
{
	const float FILTER_DIAMETER = 1.0 / 300.0;
	const vec2 offsets[9] = vec2[]
	(
		vec2(-FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels to left-up of current.
		vec2(0.0f,				FILTER_DIAMETER), // Pixels upwards of current.
		vec2(FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels up-right of current.
		vec2(-FILTER_DIAMETER,	0.0f),			  // etc.
		vec2(0.0f,				0.0f),
		vec2(FILTER_DIAMETER,	0.0f),
		vec2(-FILTER_DIAMETER,	-FILTER_DIAMETER),
		vec2(0.0f,				-FILTER_DIAMETER),
		vec2(FILTER_DIAMETER,	-FILTER_DIAMETER)
	);
	const float sharpenKernel[9] = float[]
	(
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	vec3 color = vec3(0.0);
	for(int i = 0; i < 9; i++) // Sample neighbors and apply filter.
	{
		color += vec3(texture(screenTex, TexCoord + offsets[i])) * sharpenKernel[i];
	}
	return vec4(color, 1.0);
}

vec4 Blur()
{
	const float FILTER_DIAMETER = 1.0 / 300.0;
	const vec2 offsets[9] = vec2[]
	(
		vec2(-FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels to left-up of current.
		vec2(0.0f,				FILTER_DIAMETER), // Pixels upwards of current.
		vec2(FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels up-right of current.
		vec2(-FILTER_DIAMETER,	0.0f),			  // etc.
		vec2(0.0f,				0.0f),
		vec2(FILTER_DIAMETER,	0.0f),
		vec2(-FILTER_DIAMETER,	-FILTER_DIAMETER),
		vec2(0.0f,				-FILTER_DIAMETER),
		vec2(FILTER_DIAMETER,	-FILTER_DIAMETER)
	);
	const float blurKernel[9] = float[]
	(
		1.0/16,	2.0/16,	1.0/16,
		2.0/16,	4.0/16,	2.0/16,
		1.0/16,	2.0/16,	1.0/16
	);

	vec3 color = vec3(0.0);
	for(int i = 0; i < 9; i++) // Sample neighbors and apply filter.
	{
		color += vec3(texture(screenTex, TexCoord + offsets[i])) * blurKernel[i];
	}
	return vec4(color, 1.0);
}

vec4 Edge()
{
	const float FILTER_DIAMETER = 1.0 / 300.0;
	const vec2 offsets[9] = vec2[]
	(
		vec2(-FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels to left-up of current.
		vec2(0.0f,				FILTER_DIAMETER), // Pixels upwards of current.
		vec2(FILTER_DIAMETER,	FILTER_DIAMETER), // Pixels up-right of current.
		vec2(-FILTER_DIAMETER,	0.0f),			  // etc.
		vec2(0.0f,				0.0f),
		vec2(FILTER_DIAMETER,	0.0f),
		vec2(-FILTER_DIAMETER,	-FILTER_DIAMETER),
		vec2(0.0f,				-FILTER_DIAMETER),
		vec2(FILTER_DIAMETER,	-FILTER_DIAMETER)
	);
	const float edgeKernel[9] = float[]
	(
		1,	1,	1,
		1,	-8,	1,
		1,	1,	1
	);

	vec3 color = vec3(0.0);
	for(int i = 0; i < 9; i++) // Sample neighbors and apply filter.
	{
		color += vec3(texture(screenTex, TexCoord + offsets[i])) * edgeKernel[i];
	}
	return vec4(color, 1.0);
}