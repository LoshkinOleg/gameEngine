#version 440 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material
{
    sampler2D ambientMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

struct DirectionalLight
{
    vec3 dir;
	vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

uniform Material material;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;

void main()
{
    // diffuse
    float diffuseIntensity = max(dot(-dirLight.dir, Normal), 0.0);
    vec3 diffuse = texture(material.diffuseMap, TexCoord).rgb * material.diffuseColor * dirLight.diffuseColor * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(dirLight.dir, Normal));
    vec3 viewDir = normalize(viewPos - FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = texture(material.specularMap, TexCoord).rgb * material.specularColor * dirLight.specularColor * specularIntensity;

    // result
    FragColor = vec4((diffuse + specular), 1.0);
}
