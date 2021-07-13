#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT {
    vec3 w_FragPos;
    vec3 w_Normal;
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
    vs_out.w_FragPos = (aModel * vec4(aPos, 1.0)).xyz;
    const mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vs_out.w_Normal = normalMatrix * aNormal;
    gl_Position = cameraMatrix * vec4(vs_out.w_FragPos, 1.0);
}