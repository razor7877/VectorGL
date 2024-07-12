#include <fstream>

#include <utilities/glad.h>

#include "shaderManager.hpp"

ShaderManager::ShaderManager()
{
	this->UBO = {};
}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::initUniformBuffer()
{
	glGenBuffers(1, &this->UBO);

	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Make sure buffer range corresponds to actual buffer size!
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->UBO, 0, 2 * sizeof(glm::mat4));
}

void ShaderManager::updateUniformBuffer(glm::mat4 view, glm::mat4 projection)
{
	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &view[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &projection[0][0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Shader* ShaderManager::getShader(ShaderType shader)
{
	if (enumToShader.count(shader) > 0)
		return enumToShader[shader];

	switch (shader)
	{
		case ShaderType::PHONG:
			enumToShader[shader] = new Shader("shaders/phong.vert", "shaders/phong.frag");
			break;

		case ShaderType::PBR:
			enumToShader[shader] = new Shader("shaders/pbr.vert", "shaders/pbr.frag");
			break;

		case ShaderType::SKYBOX:
			enumToShader[shader] = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
			break;

		case ShaderType::GRID:
			enumToShader[shader] = new Shader("shaders/grid.vert", "shaders/grid.frag");
			break;

		case ShaderType::GRID2:
			enumToShader[shader] = new Shader("shaders/grid2.vert", "shaders/grid2.frag");
			break;

		case ShaderType::HDRTOCUBEMAP:
			enumToShader[shader] = new Shader("shaders/cubemap.vert", "shaders/equirectangularToCubeMap.frag");
			break;

		case ShaderType::IRRADIANCE:
			enumToShader[shader] = new Shader("shaders/cubemap.vert", "shaders/irradiance.frag");
			break;
	}

	unsigned int UBIShader = glGetUniformBlockIndex(enumToShader[shader]->ID, "Matrices");
	glUniformBlockBinding(enumToShader[shader]->ID, UBIShader, 0);

	return enumToShader[shader];
}

std::string ShaderManager::getVertexShaderContent(ShaderType shader)
{
	if (enumToShader.count(shader) == 0)
		return "";

	std::ifstream t(enumToShader[shader]->vertexPath);
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		return str;
	}

	return "";
}

std::string ShaderManager::getFragmentShaderContent(ShaderType shader)
{
	if (enumToShader.count(shader) == 0)
		return "";

	std::ifstream t(enumToShader[shader]->fragmentPath);
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		return str;
	}

	return "";
}

void ShaderManager::setVertexShaderContent(ShaderType shader, std::string content)
{
	if (enumToShader.count(shader) == 0)
		return;

	std::ofstream ofs(enumToShader[shader]->vertexPath, std::ofstream::out);

	ofs << content;

	ofs.close();
}

void ShaderManager::setFragmentShaderContent(ShaderType shader, std::string content)
{
	if (enumToShader.count(shader) == 0)
		return;

	std::ofstream ofs(enumToShader[shader]->fragmentPath, std::ofstream::out);

	ofs << content;

	ofs.close();
}
