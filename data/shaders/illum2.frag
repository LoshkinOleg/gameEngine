#version 440 core

layout (location = 0) out vec4 FragColor;

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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0)); // Constant (-1;-1;-1) light direction for illum mode 2.

void main()
{
    vec3 ambient = texture(material.ambientMap, TexCoord).rgb * material.ambientColor;
    vec3 diffuse = texture(material.diffuseMap, TexCoord).rgb * material.diffuseColor * max(dot(-lightDir, Normal), 0.0); // Phong.

    // specular
    const vec3 viewDir = normalize(viewPos - FragPos);
    const vec3 halfDir = normalize(-lightDir + viewDir);
    vec3 specular =
        texture(material.specularMap, TexCoord).rgb *
        material.specularColor *
        pow(max(dot(Normal, halfDir), 0.0), material.shininess); // Blinn.

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}