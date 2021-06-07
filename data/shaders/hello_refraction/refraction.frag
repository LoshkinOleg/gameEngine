#version 440 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube environmentMap;

void main()
{    
    const float ratio = 1.0 / 1.52; // For refraction between air -> glass.
    const vec3 viewDir = normalize(Position - cameraPos);
    const vec3 refractionDir = refract(viewDir, normalize(Normal), ratio);
    FragColor = vec4(texture(environmentMap, refractionDir).rgb, 1.0);
}