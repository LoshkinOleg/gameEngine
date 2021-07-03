/*
    Implementation of a single pass bloom effect using kernels to apply a blur on the high luminosity areas.
    Very simple but to obtain convincing results, the kernel needs to be very large, which is probably rather costly.

    Note that interestingly enough, a simple mean blur is both simpler from a computation point of view AND gives better results.
    This is because a gaussian blur tends to preserve edges, which is the opposite of what we want a bloom shader to look like.
    A gaussian blur might be useful however for very faceted and transparent objects like a cut diamond since the phantom edges produced by a gaussian blur when texelSizeFactor is large give
    an apparance somewhat similar to intense light being refracted through a faceted diamond.
*/

// TODO: figure out why the bloom darkens the outside of edges.

#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture0; // FragColor
uniform sampler2D fbTexture1; // BrightColor

const int meanBlurMatrixOrder = 23;
const float gaussianKernel3x3[9] = float[]
(
    1.0 / 16.0,  2.0 / 16.0,  1.0 / 16.0,
    2.0 / 16.0,  4.0 / 16.0,  2.0 / 16.0,
    1.0 / 16.0,  2.0 / 16.0,  1.0 / 16.0
);
const float gaussianKernel5x5[25] = float[]
(
    1.0 / 256.0,    4.0  / 256.0,    6.0 / 256.0,    4.0 / 256.0,   1.0 / 256.0,
    4.0 / 256.0,    16.0 / 256.0,   24.0 / 256.0,   16.0 / 256.0,   4.0 / 256.0,
    6.0 / 256.0,    24.0 / 256.0,   36.0 / 256.0,   24.0 / 256.0,   6.0 / 256.0,
    4.0 / 256.0,    16.0 / 256.0,   24.0 / 256.0,   16.0 / 256.0,   4.0 / 256.0,
    1.0 / 256.0,    4.0  / 256.0,    6.0 / 256.0,    4.0 / 256.0,   1.0 / 256.0
);
const float gaussianKernel7x7[49] = float[]
(
    0.0 / 1068.0,      0.0  / 1068.0,     0.0   / 1068.0,    5.0   / 1068.0,    0.0   / 1068.0,    0.0  / 1068.0,     0.0 / 1068.0,
    0.0 / 1068.0,      5.0  / 1068.0,     18.0  / 1068.0,    32.0  / 1068.0,    18.0  / 1068.0,    5.0  / 1068.0,     0.0 / 1068.0,
    0.0 / 1068.0,      18.0 / 1068.0,     64.0  / 1068.0,    100.0 / 1068.0,    64.0  / 1068.0,    18.0 / 1068.0,     0.0 / 1068.0,
    5.0 / 1068.0,      32.0 / 1068.0,     100.0 / 1068.0,    100.0 / 1068.0,    100.0 / 1068.0,    32.0 / 1068.0,     5.0 / 1068.0,
    0.0 / 1068.0,      18.0 / 1068.0,     64.0  / 1068.0,    100.0 / 1068.0,    64.0  / 1068.0,    18.0 / 1068.0,     0.0 / 1068.0,
    0.0 / 1068.0,      5.0  / 1068.0,     18.0  / 1068.0,    32.0  / 1068.0,    18.0  / 1068.0,    5.0  / 1068.0,     0.0 / 1068.0,
    0.0 / 1068.0,      0.0  / 1068.0,     0.0   / 1068.0,    5.0   / 1068.0,    0.0   / 1068.0,    0.0  / 1068.0,     0.0 / 1068.0
);

vec3 Reinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 ExtendedReinhard(vec3 color, float maxWhite)
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

vec4 GaussianBlur(float texelSizeFactor)
{
    const vec4 color = texture(fbTexture0, TexCoord);
    if (color.a == 0.0) discard;

    vec2 texelSize = 1.0 / textureSize(fbTexture1, 0);
    texelSize *= texelSizeFactor;
    vec4 blurredSamples3x3[9];
    vec4 blurredSamples5x5[25];
    vec4 blurredSamples7x7[49];

    for(int y = 0; y < 7; y++)
    {
        for(int x = 0; x < 7; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 3,y - 3));
            blurredSamples7x7[y * 7 + x] = texture(fbTexture1, sampledCoord) * gaussianKernel7x7[y * 7 + x];
        }   
    }
    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 2,y - 2));
            blurredSamples5x5[y * 5 + x] = texture(fbTexture1, sampledCoord) * gaussianKernel5x5[y * 5 + x];
        }   
    }
    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 1,y - 1));
            blurredSamples3x3[y * 3 + x] = texture(fbTexture1, sampledCoord) * gaussianKernel3x3[y * 3 + x];
        }   
    }
    vec4 accumulatedBlur = vec4(0.0);
    for(int i = 0; i < 9; i++)
    {
        accumulatedBlur += blurredSamples3x3[i] * (2.0 / 3.0);
    }
    for(int i = 0; i < 25; i++)
    {
        accumulatedBlur += blurredSamples5x5[i] * (0.7 / 3.0);
    }
    for(int i = 0; i < 49; i++)
    {
        accumulatedBlur += blurredSamples7x7[i] * (0.3 / 3.0);
    }

    const vec4 result = color + accumulatedBlur;
	return result;
}

vec4 MeanBlur()
{
    const vec4 color = texture(fbTexture0, TexCoord);
    // if (color.a == 0.0) discard; // Process only pixels that have a non zero alpha.

    vec2 texelSize = 1.0 / textureSize(fbTexture1, 0);
    vec3 blurredSamples[meanBlurMatrixOrder * meanBlurMatrixOrder];

    for(int y = 0; y < meanBlurMatrixOrder; y++)
    {
        for(int x = 0; x < meanBlurMatrixOrder; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - floor(meanBlurMatrixOrder * 0.5), y - floor(meanBlurMatrixOrder * 0.5)));
            blurredSamples[y * meanBlurMatrixOrder + x] = texture(fbTexture1, sampledCoord).rgb / (meanBlurMatrixOrder * meanBlurMatrixOrder);
        }   
    }

    vec3 accumulatedBlur = vec3(0.0);
    for(int i = 0; i < meanBlurMatrixOrder * meanBlurMatrixOrder; i++)
    {
        accumulatedBlur += blurredSamples[i];
    }

    const vec3 result = color.rgb + accumulatedBlur;
	return vec4(result, color.a);
}

void main()
{
    FragColor = MeanBlur();
}