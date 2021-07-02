#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

struct VS_OUT
{
    vec3 w_Position;
    vec3 w_Normal;
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
    vs_out.w_Normal = mat3(transpose(inverse(aModel))) * aNormal;
    vs_out.w_Position = vec3(aModel * vec4(aPos, 1.0));
    gl_Position = cameraMatrix * vec4(vs_out.w_Position, 1.0);
}