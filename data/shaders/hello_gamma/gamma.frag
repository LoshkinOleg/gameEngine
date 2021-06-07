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
vec4 ComputeDirLightWithInputCorrection();

void main()
{   
    const float GAMMA = 2.2;
    if (FragPos.x > 0.3) // Non corrected.
    {
        FragColor = ComputeDirLight();
    }
    else if(FragPos.x <= 0.3 && FragPos.x > -0.3) // Gamma corrected without correcting input textures.
    {
        FragColor = vec4(pow(ComputeDirLight().rgb, vec3(1.0/GAMMA)).rgb, 1.0); // Remove the monitor's gamma.
    }
    else // Gamma corrected and input textures corrected.
    {
        FragColor = vec4(pow(ComputeDirLightWithInputCorrection().rgb, vec3(1.0/GAMMA)).rgb, 1.0); // Convert texture color back into sRGB color space.
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

vec4 ComputeDirLightWithInputCorrection()
{
    const float GAMMA = 2.2;

    // ambient
    vec3 ambient = mat.ambientColor;
    ambient = pow(ambient, vec3(GAMMA));
    ambient *= dirLight.ambient;

    // diffuse
    float diffuseIntensity = max(dot(-dirLight.dir, Normal), 0.0);
    vec3 diffuse = texture(mat.diffuseMap, TexCoord).rgb;
    diffuse = pow(diffuse, vec3(GAMMA)); // Convert the input texture from sRGB color space into linear color space for calculations.
    diffuse *= mat.diffuseColor * dirLight.diffuse * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(dirLight.dir, Normal));
    vec3 viewDir = normalize(viewPos - FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specularMap, TexCoord).rgb; // Don't color correct the specular, it's in linear space already.
    specular *= mat.specularColor * dirLight.specular * specularIntensity;

    // result
    return vec4((ambient + diffuse + specular).rgb, 1.0);
}