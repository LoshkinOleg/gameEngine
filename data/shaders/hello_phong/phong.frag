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
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

struct SpotLight
{
    vec3 pos;
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float innerCutoff;
    float outerCutoff;
};

uniform Material mat;
uniform DirectionalLight dirLight;
uniform PointLight ptLight;
uniform SpotLight spotLight;
uniform vec3 viewPos;

vec3 ComputeDirectionalLight(DirectionalLight light);
vec3 ComputePointLight(PointLight light);
vec3 ComputeSpotLight(SpotLight light);
vec3 ComputeAmbientLight();

void main()
{
    vec3 result = ComputeDirectionalLight(dirLight);
    result += ComputePointLight(ptLight);
    result += ComputeSpotLight(spotLight);
    FragColor = vec4(result, 1.0);
}

vec3 ComputeDirectionalLight(DirectionalLight light)
{
    // ambient
    vec3 ambient = texture(mat.diffuse, fs_in.TexCoord).rgb * light.ambient;

    // diffuse
    float diffuseIntensity = max(dot(-light.dir, fs_in.Normal), 0.0);
    vec3 diffuse = texture(mat.diffuse, fs_in.TexCoord).rgb * light.diffuse * diffuseIntensity;

    // specular
    vec3 reflectDir = normalize(reflect(light.dir, fs_in.Normal));
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specular, fs_in.TexCoord).rgb * light.specular * specularIntensity;

    // result
    return (ambient + diffuse + specular);
}

vec3 ComputePointLight(PointLight light)
{
    // attenuation
    float distanceToLight = length(light.pos - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

    // ambient
    vec3 ambient = texture(mat.diffuse, fs_in.TexCoord).rgb * light.ambient * attenuation;

    // diffuse
    vec3 lightDir = normalize(light.pos - fs_in.FragPos);
    float diffuseIntensity = max(dot(lightDir, fs_in.Normal), 0.0);
    vec3 diffuse = texture(mat.diffuse, fs_in.TexCoord).rgb * light.diffuse * diffuseIntensity * attenuation;

    // specular
    vec3 reflectDir = normalize(reflect(-lightDir, fs_in.Normal)); // light -> frag -> normal
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specular, fs_in.TexCoord).rgb * light.specular * specularIntensity * attenuation;

    // result
    return (ambient + diffuse + specular);
}

vec3 ComputeSpotLight(SpotLight light)
{
    // attenuation
    float distanceToLight = length(light.pos - fs_in.FragPos);
    float attenuation = 1.0 / light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight;

    // cutoff
    vec3 lightDir = normalize(light.pos - fs_in.FragPos);
    float theta = dot(lightDir, -light.dir);
    float epsilon = light.innerCutoff - light.outerCutoff;
    float cutoff = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // ambient
    vec3 ambient = texture(mat.diffuse, fs_in.TexCoord).rgb * light.ambient * attenuation * cutoff;

    // diffuse
    float diffuseIntensity = max(dot(lightDir, fs_in.Normal), 0.0);
    vec3 diffuse = texture(mat.diffuse, fs_in.TexCoord).rgb * light.diffuse * diffuseIntensity * attenuation * cutoff;

    // specular
    vec3 reflectDir = normalize(reflect(-lightDir, fs_in.Normal)); // light -> frag -> normal
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = texture(mat.specular, fs_in.TexCoord).rgb * light.specular * specularIntensity * attenuation * cutoff;

    // result
    return (ambient + diffuse + specular);
}

vec3 ComputeAmbientLight()
{
    return texture(mat.diffuse, fs_in.TexCoord).rgb;
}