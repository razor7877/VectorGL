#version 330 core

// Basic, single color shader, used for drawing debug raycasts for example

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
}
