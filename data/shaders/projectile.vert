#version 440 core

layout (location = 0) in vec2 aPos; // Per-instance, world space.

uniform mat4 PROJECTION;
uniform mat4 view;
uniform float timer;
uniform float NR_OF_VERTICES;
uniform float EXPLOSION_RADIUS_MULTIPLIER;

const float TWO_PI = 2.0 * 3.14;

float RemapToRange(const float inputRangeLower, const float inputRangeUpper, const float outputRangeLower, const float outputRangeUpper, const float value)
{
	return outputRangeLower + (value - inputRangeLower) * (outputRangeUpper - outputRangeLower) / (inputRangeUpper - inputRangeLower);
}

float Random(const float factor) // Generates a pseudo random number between 0 and 1.
{
	// Taken from: https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
    return fract(sin(dot(vec2(factor), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	vec2 pos = aPos;
	if (timer < 0.0) // < 0 means projectile has passed into the particles state of it's lifetime (explosion).
	{
		// TODO: really need the normalize?
		const vec2 dir = normalize(vec2(
			sin(RemapToRange(0.0, NR_OF_VERTICES, 0.0, TWO_PI, float(gl_VertexID))),
			cos(RemapToRange(0.0, NR_OF_VERTICES, 0.0, TWO_PI, float(gl_VertexID)))));
		pos += dir * -timer * Random(timer) * EXPLOSION_RADIUS_MULTIPLIER; // Value of timer is in the negatives here. Invert it to have an explosion instead of an implosion.
	}
	gl_Position = PROJECTION * view * vec4(pos, 0.0, 1.0);
}