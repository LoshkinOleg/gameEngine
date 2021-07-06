#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT {
    vec3 t_FragPos;
    vec2 TexCoords;
    vec3 t_LightDir;
    vec3 t_ViewPos;
} vs_out;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, 1.0));

void main()
{
    const vec4 fragPos = aModel * vec4(aPos, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.t_FragPos  = TBN * fragPos.xyz;
    vs_out.TexCoords = aTexCoord;
    vs_out.t_LightDir = TBN * lightDir;
    vs_out.t_ViewPos  = TBN * viewPos;
        
    gl_Position = cameraMatrix * fragPos;
}