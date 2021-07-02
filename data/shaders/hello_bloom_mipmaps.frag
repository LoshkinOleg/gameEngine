#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fbTexture0; // FragColor
uniform sampler2D fbTexture1; // BrightColor

const int meanBlurMatrixOrder = 3;

vec3 Reinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 ExtendedReinhard(vec3 color, float maxWhite)
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

vec3 MeanBlur(float mipmapLevel)
{
    vec2 texelSize = 1.0 / textureSize(fbTexture1, 0);
    vec3 blurredSamples[meanBlurMatrixOrder * meanBlurMatrixOrder];

    for(int y = 0; y < meanBlurMatrixOrder; y++)
    {
        for(int x = 0; x < meanBlurMatrixOrder; x++)
        {
            vec2 sampledCoord = TexCoord + (texelSize * vec2(x - floor(meanBlurMatrixOrder * 0.5), y - floor(meanBlurMatrixOrder * 0.5)));
            blurredSamples[y * meanBlurMatrixOrder + x] = texture(fbTexture1, sampledCoord, mipmapLevel).rgb / (meanBlurMatrixOrder * meanBlurMatrixOrder);
        }   
    }

    vec3 result = vec3(0.0);
    for(int i = 0; i < meanBlurMatrixOrder * meanBlurMatrixOrder; i++)
    {
        result += blurredSamples[i];
    }

    const vec3 color = texture(fbTexture0, TexCoord).rgb + result;
	return color;
}

void main()
{
    vec3 brightColor = MeanBlur(4.0) * 0.25;
    brightColor += MeanBlur(3.0) * 0.25;
    brightColor += MeanBlur(2.0) * 0.25;
    brightColor += MeanBlur(1.0) * 0.25;

    vec3 fullColor = texture(fbTexture0, TexCoord).rgb;

    FragColor = vec4(ExtendedReinhard(fullColor + brightColor, 1.0), 1.0);
}