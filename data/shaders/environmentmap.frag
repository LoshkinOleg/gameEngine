#version 440 core
out vec4 FragColor;

struct VS_OUT
{
    vec3 w_Position;
    vec3 w_Normal;
} fs_in;

uniform vec3 viewPos;
uniform samplerCube cubemap;

void main()
{    
    const vec3 viewDir = normalize(fs_in.w_Position - viewPos);
    const vec3 reflection = reflect(viewDir, normalize(fs_in.w_Normal));
    FragColor = vec4(texture(cubemap, reflection).rgb, 1.0);
}