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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material mat;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;

vec4 ComputeDirLight();

void main()
{   
    const float GAMMA = 2.2;
    if (FragPos.x > 0.0) // Non corrected.
    {
        FragColor = ComputeDirLight();
    }
    else // Gamma corrected.
    {
        FragColor = vec4(pow(ComputeDirLight().rgb, vec3(1.0/GAMMA)).rgb, 1.0);
    }
}

vec4 ComputeDirLight()
{
    // ambient
    vec3 ambient = mat.ambientColor * dirLight.ambient;

    // diffuse
    float diffuseIntensity = max(dot(-dirLight.dir, Normal), 0.0);
    vec3 diffuse = texture(mat.diffuseMap, TexCoord).rgb * mat.diffuseColor * dirLight.diffuse * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(dirLight.dir, Normal));
    vec3 viewDir = normalize(viewPos - FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specularMap, TexCoord).rgb * mat.specularColor * dirLight.specular * specularIntensity;

    // result
    return vec4((ambient + diffuse + specular).rgb, 1.0);
}