#version 440 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube environmentMap;

void main()
{    
    vec3 viewDir = normalize(Position - cameraPos);
    vec3 reflection = reflect(viewDir, normalize(Normal));
    FragColor = vec4(texture(environmentMap, reflection).rgb, 1.0);
}