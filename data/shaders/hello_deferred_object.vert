#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT {
    vec4 w_FragPos;
    vec2 TexCoords;
    vec3 w_Normal;
    vec3 w_Tangent;
    mat3 TangentToWorld;
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
    const mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TangentToWorld = mat3(T, B, N);

    vs_out.w_FragPos = aModel * vec4(aPos, 1.0);   
    vs_out.TexCoords = aTexCoord;
    vs_out.w_Normal = N;
    vs_out.w_Tangent = T;

    gl_Position = cameraMatrix * vs_out.w_FragPos;
}