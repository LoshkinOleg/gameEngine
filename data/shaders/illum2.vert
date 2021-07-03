#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
    vs_out.FragPos = vec3(aModel * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightDir = TBN * lightDir;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
        
    gl_Position = cameraMatrix * aModel * vec4(aPos, 1.0);
}