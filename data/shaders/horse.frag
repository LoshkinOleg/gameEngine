#version 440 core

layout (location = 0) out vec4 fbTexture0; // rgb: albedo
layout (location = 1) out vec4 fbTexture1; // xyz: w_FragPos
layout (location = 2) out vec4 fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
layout (location = 3) out vec4 fbTexture3; // x: shininess

in VS_OUT {
    vec3 w_FragPos;
    vec2 TexCoords;
    mat3 TBN; // Have to perform TBN multiplication in fragment shader since TexCoords gets interpolated.
} fs_in;

struct Material
{
    sampler2D normalMap;
    sampler2D diffuseMap;
    float shininess;
};

uniform Material material;

void main()
{
    fbTexture0 = vec4(1.0);
    fbTexture1 = vec4(1.0);
    fbTexture2 = vec4(1.0);
    fbTexture3 = vec4(1.0);

    // Albedo.
    fbTexture0.rgb = pow(texture(material.diffuseMap, fs_in.TexCoords).rgb, vec3(2.2)); // TODO: specify we're in sRGB when generating ktx

    // FragPos.
    fbTexture1.xyz = fs_in.w_FragPos;

    // Normals. Retrieve from normalmap, remap to range [-1;1] and convert to world space.
    fbTexture2.xyz = fs_in.TBN * normalize((texture(material.normalMap, fs_in.TexCoords).rgb) * 2.0 - 1.0);

    // Shininess.
    fbTexture3.x = material.shininess;
}