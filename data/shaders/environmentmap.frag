#version 440 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 viewPos;
uniform samplerCube cubemap;

void main()
{    
    vec3 viewDir = normalize(Position - viewPos);
    vec3 reflection = reflect(viewDir, normalize(Normal));
    FragColor = vec4(texture(cubemap, reflection).rgb, 1.0);
}