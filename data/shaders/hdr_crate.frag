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

struct PointLight
{
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform PointLight light0;
uniform PointLight light1;
uniform vec3 viewPos;

vec3 ComputePointLight(PointLight light);

void main()
{   
    vec3 result = ComputePointLight(light0);
    result += ComputePointLight(light1);
    FragColor = vec4(result, 1.0);
}

vec3 ComputePointLight(PointLight light)
{
    // attenuation
    float distanceToLight = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

    // ambient
    vec3 ambient = texture(material.diffuseMap, TexCoord).rgb * light.ambient * attenuation;

    // diffuse
    vec3 lightDir = normalize(light.pos - FragPos);
    float diffuseIntensity = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = texture(material.diffuseMap, TexCoord).rgb * light.diffuse * diffuseIntensity * attenuation;

    // specular
    vec3 reflectDir = normalize(reflect(-lightDir, -Normal)); // light -> frag -> normal
    vec3 viewDir = normalize(viewPos - FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = texture(material.specularMap, TexCoord).rgb * light.specular * specularIntensity * attenuation;

    // result
    return (ambient + diffuse + specular);
}