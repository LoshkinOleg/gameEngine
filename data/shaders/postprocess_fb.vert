#version 440 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;
out float MaxBrightness; 

uniform sampler2D fbTexture0;

const float maxBrightness = 1.0;

void main()
{
    // Turns out, this is a terrible idea, both in performance and in resulting effect. To have smooth transitions, previous luminosities need to be taken into account.
	// Find brightest pixel.
    // float maxBrightness = 0.0;
    // const ivec2 size = textureSize(fbTexture0, 4);
    // const vec2 texelSize = 1.0 / size;
    // for (int y = 0; y < size.y; y++)
    // {
    //     for (int x = 0; x < size.x; x++)
    //     {
    //         const float sampled = dot(texture(fbTexture0, vec2(x, y) * texelSize).rgb, vec3(0.2126, 0.7152, 0.0722));
    //         maxBrightness = max(sampled, maxBrightness);
    //     }
    // }

    MaxBrightness = maxBrightness;
	TexCoord = aPos * 0.5 + 0.5;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}