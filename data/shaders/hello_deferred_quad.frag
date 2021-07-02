#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

struct PointLight
{
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

uniform sampler2D fbTexture0; // Position
uniform sampler2D fbTexture1; // AlbedoAndSpec
uniform sampler2D fbTexture2; // Normals

uniform vec3 viewPos;
uniform PointLight lights[32];

vec3 Reinhard(vec3 color)
{
    return color / (1.0 + color);
}

vec3 ExtendedReinhard(vec3 color, float maxWhite)
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

void main()
{
    const vec3 FragPos = texture(fbTexture0, TexCoord).rgb;
    const vec3 Color = texture(fbTexture1, TexCoord).rgb;
    const vec3 SpecularColor = vec3(texture(fbTexture0, TexCoord).a);
    const vec3 Normal = texture(fbTexture2, TexCoord).rgb;
    const vec3 viewDir = normalize(viewPos - FragPos);
    const float ambientMultiplier = 0.2;
    const float shininess = 64.0;

    // Ambient.
    vec3 color = Color * ambientMultiplier;

    for(int i = 0; i < 32; i++)
    {
        const float distToLight = length(lights[i].position - FragPos);
        if (distToLight < lights[i].radius)
        {
            const float attenuation = 1.0 / (1.0 + lights[i].linear * distToLight + lights[i].quadratic * distToLight * distToLight);

            // Diffuse.
            const vec3 lightDir = normalize(lights[i].position - FragPos);
            color += Color * max(dot(Normal, -lightDir), 0.0) * lights[i].color * attenuation;

            // Specular.
            const vec3 halfwayDir = normalize(lightDir + viewDir);
            color += SpecularColor * pow(max(dot(Normal, halfwayDir), 0.0), shininess) * lights[i].color * attenuation;
        }
    }
    FragColor = vec4(ExtendedReinhard(color, 0.5), 1.0);
}