#version 440 core
out vec4 FragColor;

in vec2 TexCoord;
in float MaxBrightness;

uniform sampler2D fbTexture0; // Color.
uniform sampler2D fbTexture1; // Brights.

const float GAMMA = 2.2;
const int MEAN_BLUR_MAXTRIX_ORDER = 8;

vec3 ExtendedReinhard(vec3 color, float maxWhite) // TODO: move this to post process stage.
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

vec3 MeanBlur()
{
    const vec3 color = texture(fbTexture0, TexCoord).rgb;

    const vec2 texelSize = 1.0 / textureSize(fbTexture1, 0);
    vec3 blurredSamples[MEAN_BLUR_MAXTRIX_ORDER * MEAN_BLUR_MAXTRIX_ORDER];

    for(int y = 0; y < MEAN_BLUR_MAXTRIX_ORDER; y++)
    {
        for(int x = 0; x < MEAN_BLUR_MAXTRIX_ORDER; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - floor(MEAN_BLUR_MAXTRIX_ORDER * 0.5), y - floor(MEAN_BLUR_MAXTRIX_ORDER * 0.5)));
            blurredSamples[y * MEAN_BLUR_MAXTRIX_ORDER + x] = texture(fbTexture1, sampledCoord).rgb / (MEAN_BLUR_MAXTRIX_ORDER * MEAN_BLUR_MAXTRIX_ORDER);
        }
    }

    vec3 accumulatedBlur = vec3(0.0);
    for(int i = 0; i < MEAN_BLUR_MAXTRIX_ORDER * MEAN_BLUR_MAXTRIX_ORDER; i++)
    {
        accumulatedBlur += blurredSamples[i];
    }

    return color.rgb + accumulatedBlur;
}

void main()
{
    vec3 result = MeanBlur();
    result = ExtendedReinhard(result, MaxBrightness);
    result = pow(result, vec3(1.0/GAMMA));
    FragColor = vec4(result, 1.0);
}