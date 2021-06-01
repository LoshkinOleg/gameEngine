#version 440 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

struct Material
{
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

vec3 ComputeDirectionalLight(DirectionalLight light);
vec3 ComputeAmbientLight();

void main()
{
    FragColor = vec4(ComputeDirectionalLight(dirLight), 1.0);
    // FragColor = vec4(ComputeAmbientLight(), 1.0);
}

vec3 ComputeDirectionalLight(DirectionalLight light)
{
    // ambient
    vec3 ambient = mat.ambientColor * light.ambient;

    // diffuse
    float diffuseIntensity = max(dot(-light.dir, fs_in.Normal), 0.0);
    vec3 diffuse = texture(mat.diffuseMap, fs_in.TexCoord).rgb * mat.diffuseColor * light.diffuse * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(light.dir, fs_in.Normal));
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specularMap, fs_in.TexCoord).rgb * mat.specularColor * light.specular * specularIntensity;

    // result
    return (ambient + diffuse + specular);
}

vec3 ComputeAmbientLight()
{
    return texture(mat.diffuseMap, fs_in.TexCoord).rgb;
}