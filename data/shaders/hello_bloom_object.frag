#version 440 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material
{
    sampler2D ambientMap;
    sampler2D albedoMap;
    sampler2D alphaMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D roughnessMap;
    sampler2D metallicMap;
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

uniform float lightMultiplier = 15.0;

void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    
    const vec3 color = texture(material.diffuseMap, fs_in.TexCoords).rgb;
    const float alpha = texture(material.alphaMap, fs_in.TexCoords).r;

    // ambient
    const float ambientIntensity = 0.1;
    vec3 ambient = ambientIntensity * color;
    // diffuse
    float diffuseIntensity = max(dot(-fs_in.TangentLightDir, normal), 0.0);
    vec3 diffuse = diffuseIntensity * color;
    diffuse *= lightMultiplier;

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(fs_in.TangentLightDir, normal);
    vec3 halfwayDir = normalize(-fs_in.TangentLightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 specular = spec * texture(material.specularMap, fs_in.TexCoords).rgb;
    specular *= lightMultiplier;

    FragColor.rgb = ambient + diffuse + specular;
    FragColor.a = alpha;
    
    const float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    BrightColor = brightness > 1.0 ? FragColor : vec4(0.0, 0.0, 0.0, 0.0);
}