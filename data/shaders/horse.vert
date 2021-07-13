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
    vec3 w_FragPos;
    vec2 TexCoords;
    mat3 TBN; // Have to perform TBN multiplication in fragment shader since TexCoords gets interpolated.
} vs_out;

uniform mat4 cameraMatrix;
uniform float interpolationFactor = 0.5;

void main()
{
    const mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * mix(aTangent0, aTangent1, interpolationFactor));
    vec3 N = normalize(normalMatrix * mix(aNormal0, aNormal1, interpolationFactor));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);

    vs_out.w_FragPos  = (aModel * vec4(mix(aPos0, aPos1, interpolationFactor), 1.0)).xyz;
    vs_out.TexCoords = aTexCoord;

    gl_Position = cameraMatrix * vec4(vs_out.w_FragPos, 1.0); // Need to write to gl_Position to allow the pipeline to clip fragments that are offscreen.
}