#version 440 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
	vec3 t_FragPos;
	vec4 l_FragPos;
	vec3 t_LightDir;
	vec3 t_ViewPos;
} fs_in;

struct Material
{
    sampler2D alphaMap;
    sampler2D normalMap;
    sampler2D ambientMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform Material material;
uniform sampler2D fbTexture0;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(fbTexture0, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    // vec3 normal = normalize(fs_in.Normal);
    // float bias = max(0.05 * (1.0 - dot(normal, -fs_in.LightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth /* - bias*/ > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(fbTexture0, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(fbTexture0, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth /*- bias*/ > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return 1.0 - shadow;
}

void main()
{           
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    const float alpha = texture(material.alphaMap, fs_in.TexCoords).r;
   
    // ambient
    const float ambientIntensity = 0.2;
    vec4 ambient = vec4((ambientIntensity * texture(material.ambientMap, fs_in.TexCoords).rgb) , alpha);
    // diffuse
    float diffuseIntensity = max(dot(-fs_in.t_LightDir, normal), 0.0);
    vec4 diffuse = vec4((diffuseIntensity * texture(material.diffuseMap, fs_in.TexCoords).rgb), alpha);

    // specular
    vec3 viewDir = normalize(fs_in.t_ViewPos - fs_in.t_FragPos);
    vec3 reflectDir = reflect(fs_in.t_LightDir, normal);
    vec3 halfwayDir = normalize(-fs_in.t_LightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec4 specular = vec4((texture(material.specularMap, fs_in.TexCoords).rgb * spec), alpha);

    float directShadow = ShadowCalculation(fs_in.l_FragPos);

    FragColor = ambient + directShadow * (diffuse + specular);
}