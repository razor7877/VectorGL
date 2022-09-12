#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	// Convert view matrix to a 4x4 and back to a 3x3 to remove any translation values
	mat4 viewNoTranslate = mat4(mat3(view));
	vec4 pos = projection * viewNoTranslate * vec4(aPos, 1.0);
	gl_Position = pos.xyww;

	TexCoords = aPos;
}