#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform vec3 offsets[1000];

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	vec3 offset = offsets[gl_InstanceID];
	gl_Position = projection * view * model * vec4(aPos + offset, 1.0);
	TexCoord = aTexCoord;
}