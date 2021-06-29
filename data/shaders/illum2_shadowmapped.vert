#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT
{
	vec3 FragPos;
	vec4 LightSpaceFragPos;
    vec3 TangentFragPos;
    vec3 LightDir;
	vec3 TangentLightDir;
    vec3 ViewPos;
    vec3 TangentViewPos;
	vec2 TexCoords;
	vec3 Normal;
} vs_out;

uniform mat4 lightMatrix;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
    const mat3 normalMatrix = transpose(inverse(mat3(aModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    const vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    const vec3 B = cross(N, T);
    const mat3 TBN = transpose(mat3(T, B, N));

	vs_out.FragPos = vec3(aModel * vec4(aPos, 1.0));
	vs_out.LightSpaceFragPos = lightMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;

    vs_out.LightDir = lightDir;
    vs_out.TangentLightDir = TBN * lightDir;

    vs_out.ViewPos  = viewPos;
    vs_out.TangentViewPos  = TBN * viewPos;

	vs_out.Normal = N;
	vs_out.TexCoords = aTexCoord;

	gl_Position = projection * view * aModel * vec4(aPos, 1.0);
}