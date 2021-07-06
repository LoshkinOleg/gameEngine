#version 440 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 t_FragPos;
    vec2 TexCoords;
    vec3 t_LightDir;
    vec3 t_ViewPos;
} fs_in;

struct Material
{
    sampler2D alphaMap;
    sampler2D normalMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform Material material;

const float ambientIntensity = 0.2;

void main()
{
    // Get the normal from normal map and convert it to [0;1] range.
    const vec3 t_normal = normalize((texture(material.normalMap, fs_in.TexCoords).rgb) * 2.0 - 1.0);
    const float alpha = texture(material.alphaMap, fs_in.TexCoords).r;
    const vec3 color = texture(material.diffuseMap, fs_in.TexCoords).rgb;
    const vec3 specularColor = texture(material.specularMap, fs_in.TexCoords).rgb;
    const vec3 t_viewDir = normalize(fs_in.t_ViewPos - fs_in.t_FragPos);
    vec3 t_reflectDir = reflect(fs_in.t_LightDir, t_normal); // Should return a normalized direction.
    vec3 t_halfwayDir = normalize(t_viewDir - fs_in.t_LightDir);
   
    vec3 ambient = ambientIntensity * color;

    const float diffuseIntensity = max(dot(-fs_in.t_LightDir, t_normal), 0.0);
    const vec3 diffuse = diffuseIntensity * color;

    const float specularIntensity = pow(max(dot(t_normal, t_halfwayDir), 0.0), material.shininess);
    const vec3 specular = specularIntensity * specularColor;

    FragColor = vec4(ambient + diffuse + specular, alpha);
}