#version 440 core
layout (location = 0) out vec4 FragColor; // Color
layout (location = 1) out vec4 BrightColor; // Brights

in vec2 TexCoord;

uniform sampler2D fbTexture0; // rgb: albedo, w: shininess
uniform sampler2D fbTexture1; // xyz: w_FragPos
uniform sampler2D fbTexture2; // xyz: w_Normal normalized and in range [-1;1]
uniform sampler2D fbTexture3; // x: shininess
// uniform sampler2D fbTexture3;
// uniform sampler2D fbTexture4;
// uniform sampler2D fbTexture5;

uniform vec3 viewPos;

const float GAMMA = 2.2;
const vec3 LIGHT_DIR = normalize(vec3(1.0, -1.0, -1.0));
const float LIGHT_INTENSITY = 1.0;
const vec3 SPECULAR_COLOR = vec3(1.0);
const float AMBIENT_FACTOR = 0.15;
const float INVALID_FLOAT = 1.0/0.0; // Allowed by glsl. Can be checked via isinf().
const vec3 HUMAN_EYE_SENSITIVITY = vec3(0.2126, 0.7152, 0.0722);

vec3 ExtendedReinhard(vec3 color, float maxWhite) // TODO: move this to post process stage.
{
	vec3 numerator = color * (1.0 + (color / vec3(maxWhite * maxWhite)));
    return numerator / (1.0 + color);
}

void main()
{
    const vec3 albedo = texture(fbTexture0, TexCoord).rgb;
    const vec3 fragPos = texture(fbTexture1, TexCoord).xyz;
    const vec3 normal = texture(fbTexture2, TexCoord).xyz;
    const float shininess = texture(fbTexture3, TexCoord).x;

    // If (1.0 / 0.0) is passed as fragPos.x, it means we're trying to render a pixel that comes from an object that shouldn't be rendered using blinn-phong, like a particle or a skybox.
    if (!isinf(fragPos.x))
    { // Default behaviour. Use blinn-phong to render the fragment as usual.
        const vec3 ambient = AMBIENT_FACTOR * albedo;

        const float diffuseInstensity = max(dot(-LIGHT_DIR, normal), 0.0);
        const vec3 diffuse = (1.0 - AMBIENT_FACTOR) * diffuseInstensity * albedo;

        const vec3 viewDir = normalize(viewPos - fragPos);
        const vec3 reflectDir = reflect(LIGHT_DIR, normal);
        const vec3 halfwayDir = normalize(-LIGHT_DIR + viewDir);
        const float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        const vec3 specular = (1.0 - AMBIENT_FACTOR) * specularIntensity * SPECULAR_COLOR;

        vec3 result = ambient + (LIGHT_INTENSITY * (diffuse + specular));
	    FragColor = vec4(result, 1.0);
    }
    else
    { // Trying to render something that blinn-phong shouldn't be applied to like particles or the skybox.
        FragColor = vec4(albedo, 1.0); // Just write the albedo as is.
    }

    const float brightness = dot(FragColor.rgb, HUMAN_EYE_SENSITIVITY);
    BrightColor = brightness > 1.0 ? FragColor : vec4(0.0);
}