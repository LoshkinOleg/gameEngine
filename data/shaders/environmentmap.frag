#version 440 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform sampler2D PosX;
uniform sampler2D NegX;
uniform sampler2D PosY;
uniform sampler2D NegY;
uniform sampler2D PosZ;
uniform sampler2D NegZ;

void main()
{    
    const vec3 viewDir = normalize(FragPos - viewPos);
    const vec3 reflection = reflect(viewDir, normalize(Normal));
    
    const float dir[3] = float[]
    (
        abs(reflection.x),
        abs(reflection.y),
        abs(reflection.z)
    );

    if (dir[0] >= dir[1] && dir[0] >= dir[2])
    {
        // x is biggest
        if (reflection.x >= 0)
        {
            // use PosX
            FragColor = texture(PosX, reflection.yz);
        }
        else
        {
            // use NegX
            FragColor = texture(NegX, reflection.yz);
        }
    }
    else if(dir[1] >= dir[0] && dir[1] >= dir[2])
    {
        // y is biggest
        if (reflection.y >= 0)
        {
            // use PosY
            FragColor = texture(PosY, reflection.xz);
        }
        else
        {
            // use NegY
            FragColor = texture(NegY, reflection.xz);
        }
    }
    else if(dir[2] >= dir[0] && dir[2] >= dir[1])
    {
        // z is biggest
        if (reflection.y >= 0)
        {
            // use PosZ
            FragColor = texture(PosZ, reflection.xy);
        }
        else
        {
            // use NegZ
            FragColor = texture(NegZ, reflection.xy);
        }
    }
}