#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat3 normalMatrix;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	vec4 viewPos = view * model * vec4(aPos, 1.0);
    gl_Position = projection * viewPos;

    FragPos = viewPos.xyz;
	TexCoord = aTexCoord;
	Normal = vec3(view * model * vec4(normalMatrix * aNormal, 1.0));

	vec3 T = normalize(vec3(view * model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(view * model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(view * model * vec4(aNormal, 0.0)));
	TBN = mat3(T, B, N);
}