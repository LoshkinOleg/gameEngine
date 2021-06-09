#version 440 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

struct DirectionalLight
{
    vec3 dir;
    vec3 diffuseColor;
    vec3 specularColor;
};

uniform Material mat;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;

void main()
{
    // diffuse
    float diffuseIntensity = max(dot(-dirLight.dir, Normal), 0.0);
    vec3 diffuse = texture(mat.diffuseMap, TexCoord).rgb * mat.diffuseColor * dirLight.diffuseColor * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(dirLight.dir, Normal));
    vec3 viewDir = normalize(viewPos - FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specularMap, TexCoord).rgb * mat.specularColor * dirLight.specularColor * specularIntensity;

    // result
    FragColor = vec4((diffuse + specular), 1.0);
}
