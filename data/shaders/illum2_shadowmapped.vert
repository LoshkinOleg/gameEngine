#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT
{
    vec2 TexCoords;
	vec3 t_FragPos;
	vec4 l_FragPos;
	vec3 t_LightDir;
	vec3 t_ViewPos;
} vs_out;

uniform mat4 lightMatrix;
uniform mat4 cameraMatrix;
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

	vs_out.l_FragPos = lightMatrix * vec4((aModel * vec4(aPos, 1.0)).rgb, 1.0);
    vs_out.t_FragPos  = TBN * (aModel * vec4(aPos, 1.0)).rgb;

    vs_out.t_LightDir = TBN * lightDir;

    vs_out.t_ViewPos  = TBN * viewPos;

	vs_out.TexCoords = aTexCoord;

	gl_Position = cameraMatrix * aModel * vec4(aPos, 1.0);
}