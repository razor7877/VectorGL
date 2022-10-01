#version 330 core

// This is the default shader for rendering most 3D objects and apply phong lighting to them
// Applies lighting if normals are given, applies a texture if it's given

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoord = aTexCoord;
	Normal = aNormal;
}