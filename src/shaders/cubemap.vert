#version 330 core

// Shader for rendering to a cubemap

layout (location = 0) in vec3 aPos;

out vec3 FragPos;

// layout (std140) uniform Matrices
// {
// 	mat4 view;
// 	mat4 projection;
// };

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
	FragPos = aPos;
}