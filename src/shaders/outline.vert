#version 330 core

// Shader for drawing object outlines

layout (location = 0) in vec3 aPos;

uniform mat4 model;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}