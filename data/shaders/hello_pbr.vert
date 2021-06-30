#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aModel;

out VS_OUT // w = world, l = light, t = tangent
{
	vec4 w_FragPos;
	vec4 l_FragPos;
    vec4 t_FragPos;
    vec3 w_LightDir;
    vec3 l_LightDir;
	vec3 t_LightDir;
    vec4 w_ViewPos;
    vec4 l_ViewPos;
    vec4 t_ViewPos;
	vec2 TexCoords;
	vec3 w_Normal;
	vec3 l_Normal;
	vec3 t_Normal;
    vec3 w_Tangent;
	vec3 l_Tangent;
	vec3 t_Tangent;
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

	vs_out.w_FragPos = aModel * vec4(aPos, 1.0);
	vs_out.l_FragPos = lightMatrix * vs_out.w_FragPos;
    vs_out.t_FragPos = vec4(TBN * vs_out.w_FragPos.rgb, vs_out.w_FragPos.w);

    vs_out.w_LightDir = lightDir;
    vs_out.l_LightDir = (lightMatrix * vec4(lightDir, 1.0)).rgb;
    vs_out.t_LightDir = TBN * lightDir;

    vs_out.w_ViewPos = vec4(viewPos, 1.0);
    vs_out.l_ViewPos = lightMatrix * vec4(viewPos, 1.0);
    vs_out.t_ViewPos = vec4(TBN * viewPos.rgb, 1.0);

    vs_out.TexCoords = aTexCoord;

    vs_out.w_Normal = N;
    vs_out.l_Normal = (lightMatrix * vec4(N, 1.0)).rgb;
    vs_out.t_Normal = TBN * N;

    vs_out.w_Tangent = T;
    vs_out.l_Tangent = (lightMatrix * vec4(T, 1.0)).rgb;
    vs_out.t_Tangent = TBN * T;

	gl_Position = projection * view * aModel * vec4(aPos, 1.0);
}