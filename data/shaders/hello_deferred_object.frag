#version 440 core

layout (location = 0) out vec4 Position;
layout (location = 1) out vec4 AlbedoAndSpec;
layout (location = 2) out vec4 Normals;

in VS_OUT {
    vec4 w_FragPos;
    vec2 TexCoords;
    vec3 w_Normal;
    vec3 w_Tangent;
    mat3 TangentToWorld;
} fs_in;

struct Material
{
    sampler2D alphaMap;
    sampler2D normalMap;
    sampler2D ambientMap;
    sampler2D albedoMap;
    sampler2D diffuseMap;
    sampler2D roughnessMap;
    sampler2D specularMap;
    sampler2D metallicMap;
    float shininess;
    float ior;
};

uniform Material material;

void main()
{
    Position = fs_in.w_FragPos;
    AlbedoAndSpec.rgb = texture(material.ambientMap, fs_in.TexCoords).rgb;
    AlbedoAndSpec.a = texture(material.specularMap, fs_in.TexCoords).r;
    Normals = vec4(fs_in.TangentToWorld * normalize(texture(material.normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0), 0.0);
}