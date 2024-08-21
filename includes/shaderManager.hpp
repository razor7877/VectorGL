#pragma once

#include <map>

#include "glm/glm.hpp"

#include "shader.hpp"

enum class ShaderType
{
	PHONG,
	PBR,
	SKYBOX,
	GRID,
	GRID2,
	HDRTOCUBEMAP,
	IRRADIANCE,
	PREFILTER,
	BRDF,
	SOLID,
	OUTLINE,
	DEPTH,
	GBUFFER,
};

/// <summary>
/// A class that manages resources associated to shaders
/// It handles creation of the shaders on the GPU as well as uniform buffers
/// </summary>
class ShaderManager
{
public:
	/// <summary>
	/// Contains all currently loaded shaders
	/// </summary>
	std::map<ShaderType, Shader*> enumToShader;

	ShaderManager();
	~ShaderManager();

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

	std::string getVertexShaderContent(ShaderType shader);
	std::string getFragmentShaderContent(ShaderType shader);

	void setVertexShaderContent(ShaderType shader, std::string content);
	void setFragmentShaderContent(ShaderType shader, std::string content);
private:
	// Uniform buffer object (for global uniforms between shaders)
	GLuint UBO;
};