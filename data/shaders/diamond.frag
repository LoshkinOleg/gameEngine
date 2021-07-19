#version 440 core

layout (location = 0) out vec4 fbTexture0; // rgb: albedo, w: shininess
layout (location = 1) out vec4 fbTexture1; // xyz: w_FragPos
layout (location = 2) out vec4 fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
layout (location = 3) out vec4 fbTexture3; // x: shininess

in VS_OUT {
    vec3 w_FragPos;
    vec3 w_Normal;
} fs_in;

uniform samplerCube cubemap;
uniform vec3 viewPos;

const float INVALID_FLOAT = 1.0/0.0; // Allowed by glsl. Can be checked via isinf().
const float COLOR_MULTIPLIER = 10.0; // To make the diamond shine and bloom.

void main()
{
    fbTexture0 = vec4(1.0);
    fbTexture1 = vec4(1.0);
    fbTexture2 = vec4(1.0);
    fbTexture3 = vec4(1.0);

    const vec3 cameraFront = normalize(fs_in.w_FragPos - viewPos);
    const vec3 reflectDir = reflect(cameraFront, fs_in.w_Normal);
    const vec3 color = vec3(pow(texture(cubemap, reflectDir).rgb, vec3(2.2)));

    fbTexture0.rgb = COLOR_MULTIPLIER * color;

    fbTexture1.xyz = vec3(INVALID_FLOAT);

    fbTexture2.xyz = vec3(INVALID_FLOAT);
    
    fbTexture3.xyz = vec3(INVALID_FLOAT);
}