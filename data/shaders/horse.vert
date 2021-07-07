#version 440 core

layout (location = 0) in vec3 aPos0;
layout (location = 1) in vec3 aPos1;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal0;
layout (location = 4) in vec3 aNormal1;
layout (location = 5) in vec3 aTangent0;
layout (location = 6) in vec3 aTangent1;
layout (location = 7) in mat4 aModel;

out VS_OUT {
    vec3 t_FragPos;
    vec2 TexCoords;
    vec3 t_LightDir;
    vec3 t_ViewPos;
} vs_out;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;
uniform float interpolationFactor = 0.5;

const vec3 lightDir = normalize(vec3(1.0, -1.0, -1.0));

void main()
{
    const vec4 fragPos = aModel * vec4(mix(aPos0, aPos1, interpolationFactor), 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * mix(aTangent0, aTangent1, interpolationFactor));
    vec3 N = normalize(normalMatrix * mix(aNormal0, aNormal1, interpolationFactor));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.t_FragPos  = TBN * fragPos.xyz;
    vs_out.TexCoords = aTexCoord;
    vs_out.t_LightDir = TBN * lightDir;
    vs_out.t_ViewPos  = TBN * viewPos;
        
    gl_Position = cameraMatrix * fragPos;
}