#version 440 core

layout (location = 0) in vec3 aPos0;
layout (location = 1) in vec3 aPos1;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal0;
layout (location = 4) in vec3 aNormal1;
layout (location = 5) in vec3 aTangent0;
layout (location = 6) in vec3 aTangent1;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentNormal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 viewPos;
uniform float interpolationFactor;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
    clamp(interpolationFactor, 0.0, 1.0);

    vs_out.FragPos = vec3(mat4(1.0) * vec4(mix(aPos0, aPos1, interpolationFactor), 1.0));   
    vs_out.TexCoords = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(mat4(1.0))));
    vec3 T = normalize(normalMatrix * mix(aTangent0, aTangent1, interpolationFactor));
    vec3 N = normalize(normalMatrix * mix(aNormal0, aNormal1, interpolationFactor));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightDir = TBN * lightDir;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.TangentNormal  = TBN * N;
        
    gl_Position = projection * view * mat4(1.0) * vec4(mix(aPos0, aPos1, interpolationFactor), 1.0);
}