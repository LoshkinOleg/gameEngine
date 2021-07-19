#version 440 core

layout (location = 0) out vec4 fbTexture0; // rgb: albedo, w: shininess
layout (location = 1) out vec4 fbTexture1; // xyz: w_FragPos
layout (location = 2) out vec4 fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
layout (location = 3) out vec4 fbTexture3; // x: shininess

in VS_OUT {
    vec3 w_FragPos;
    vec2 TexCoords;
    mat3 TBN; // Have to perform TBN multiplication in fragment shader since TexCoords gets interpolated.
    vec4 l_FragPos;
} fs_in;

struct Material
{
    sampler2D normalMap;
    sampler2D diffuseMap;
    float shininess;
};

uniform Material material;
uniform sampler2D shadowmap; // Texture unit 15, a framebuffer texture.

const float INVALID_FLOAT = 1.0/0.0; // Allowed by glsl. Can be checked via isinf().

float ComputeShadow(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowmap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth > pcfDepth  ? 1.0 : 0.0;        
        }
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    fbTexture0 = vec4(1.0);
    fbTexture1 = vec4(1.0);
    fbTexture2 = vec4(1.0);
    fbTexture3 = vec4(1.0);

    // Albedo.
    fbTexture0.rgb = pow(texture(material.diffuseMap, fs_in.TexCoords).rgb, vec3(2.2)) * (1.0 - ComputeShadow(fs_in.l_FragPos));

    // FragPos.
    fbTexture1.xyz = fs_in.w_FragPos;
    fbTexture1.x = INVALID_FLOAT;
    
    // Normals. Retrieve from normalmap, remap to range [-1;1] and convert to world space.
    fbTexture2.xyz = fs_in.TBN * normalize((texture(material.normalMap, fs_in.TexCoords).rgb) * 2.0 - 1.0);

    fbTexture3.x = material.shininess;
}