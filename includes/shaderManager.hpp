#pragma once

#include <map>

#include "glm/glm.hpp"

#include "shader.hpp"

enum class ShaderType
{
	PHONG,
	SKYBOX,
	GRID,
	GRID2,
};

/// <summary>
/// A class that manages resources associated to shaders
/// It handles creation of the shaders on the GPU as well as uniform buffers
/// </summary>
class ShaderManager
{
public:
	ShaderManager();

	/// <summary>
	/// Initializes the uniform buffer
	/// </summary>
	void initUniformBuffer();

	/// <summary>
	/// Updates the uniform buffer data
	/// </summary>
	/// <param name="view">The camera's view matrix</param>
	/// <param name="projection">The camera's projection matrix</param>
	void updateUniformBuffer(glm::mat4 view, glm::mat4 projection);

	/// <summary>
	/// Returns a shader of a given type and creates it if wasn't queried before
	/// </summary>
	/// <param name="shader">The shader type</param>
	/// <returns></returns>
	Shader* getShader(ShaderType shader);

private:
	// Uniform buffer object (for global uniforms between shaders)
	GLuint UBO;

	// To store loaded shaders
	std::map<ShaderType, Shader*> enumToShader;
};