#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT {
    vec3 w_FragPos;
    vec2 TexCoords;
    mat3 TBN; // Have to perform TBN multiplication in fragment shader since TexCoords gets interpolated.
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
    const mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);

    vs_out.w_FragPos  = (aModel * vec4(aPos, 1.0)).xyz;
    vs_out.TexCoords = aTexCoord;

    gl_Position = cameraMatrix * vec4(vs_out.w_FragPos, 1.0); // Need to write to gl_Position to allow the pipeline to clip fragments that are offscreen.
}