#version 440 core

layout (location = 0) out vec4 FragColor;

struct Material
{
    sampler2D ambientMap;
    sampler2D alphaMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D roughnessMap;
    sampler2D metallicMap;
    sampler2D sheenMap;
    sampler2D emissiveMap;
    float shininess;
    float ior;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform Material material;

// #define DEBIGGING_HINTS

void main()
{
#ifdef DEBIGGING_HINTS
    vec3 ambientColor = texture(material.ambientMap, fs_in.TexCoords).rgb;
    if (ambientColor.r < 0.01 && ambientColor.g < 0.01 && ambientColor.b < 0.01)
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }
    vec3 diffuseColor = texture(material.diffuseMap, fs_in.TexCoords).rgb;
    if (diffuseColor.r < 0.01 && diffuseColor.g < 0.01 && diffuseColor.b < 0.01)
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        return;
    }
    vec3 alphaColor = texture(material.alphaMap, fs_in.TexCoords).rgb;
    if (alphaColor.r < 0.01 && alphaColor.g < 0.01 && alphaColor.b < 0.01)
    {
        FragColor = vec4(0.0, 1.0, 1.0, 1.0);
        return;
    }
    vec3 specularColor = texture(material.specularMap, fs_in.TexCoords).rgb;
    if (specularColor.r < 0.01 && specularColor.g < 0.01 && specularColor.b < 0.01)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }
    vec3 normalColor = texture(material.normalMap, fs_in.TexCoords).rgb;
    if (normalColor.r < 0.01 && normalColor.g < 0.01 && normalColor.b < 0.01)
    {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }
#endif

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // ambient
    const float ambientIntensity = 0.2;
    vec4 ambient = vec4((ambientIntensity * texture(material.ambientMap, fs_in.TexCoords).rgb) , texture(material.alphaMap, fs_in.TexCoords).r);
    // diffuse
    float diffuseIntensity = max(dot(-fs_in.TangentLightDir, normal), 0.0);
    vec4 diffuse = vec4((diffuseIntensity * texture(material.diffuseMap, fs_in.TexCoords).rgb), texture(material.alphaMap, fs_in.TexCoords).r);

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(fs_in.TangentLightDir, normal);
    vec3 halfwayDir = normalize(-fs_in.TangentLightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec4 specular = vec4((texture(material.specularMap, fs_in.TexCoords).rgb * spec), texture(material.alphaMap, fs_in.TexCoords).r);
    FragColor = ambient + diffuse + specular;
}