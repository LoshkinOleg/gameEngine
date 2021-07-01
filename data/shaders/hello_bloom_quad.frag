#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture0; // FragColor
uniform sampler2D fbTexture1; // BrightColor

const float offset = 1.0 / 100.0;
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
    vec2 texelSize = 1.0 / textureSize(fbTexture1, 0);
    texelSize *= texelSizeFactor;
    vec3 blurredSamples3x3[9];
    vec3 blurredSamples5x5[25];
    vec3 blurredSamples7x7[49];

    for(int y = 0; y < 7; y++)
    {
        for(int x = 0; x < 7; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 3,y - 3));
            blurredSamples7x7[y * 7 + x] = texture(fbTexture1, sampledCoord).rgb * gaussianKernel7x7[y * 7 + x];
        }   
    }
    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 2,y - 2));
            blurredSamples5x5[y * 5 + x] = texture(fbTexture1, sampledCoord).rgb * gaussianKernel5x5[y * 5 + x];
        }   
    }
    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - 1,y - 1));
            blurredSamples3x3[y * 3 + x] = texture(fbTexture1, sampledCoord).rgb * gaussianKernel3x3[y * 3 + x];
        }   
    }
    vec3 result = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        result += blurredSamples3x3[i] * (2.0 / 3.0);
    }
    for(int i = 0; i < 25; i++)
    {
        result += blurredSamples5x5[i] * (0.7 / 3.0);
    }
    for(int i = 0; i < 49; i++)
    {
        result += blurredSamples7x7[i] * (0.3 / 3.0);
    }

    const vec3 color = texture(fbTexture0, TexCoord).rgb + result;
	return vec4(color, 1.0);
}

void main()
{
    FragColor = vec4(ExtendedReinhard(GaussianBlur(5.0).rgb, 1.0), 1.0);
}