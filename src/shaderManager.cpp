#include <fstream>

#include <utilities/glad.h>

#include "shaderManager.hpp"
#include "logger.hpp"

ShaderManager::ShaderManager() = default;

ShaderManager::~ShaderManager() = default;

void ShaderManager::end()
{
	glDeleteBuffers(1, &this->UBO);

	for (auto&& [type, shader] : this->enumToShader)
		delete shader;
}

void ShaderManager::initUniformBuffer()
{
	glGenBuffers(1, &this->UBO);

	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Make sure buffer range corresponds to actual buffer size!
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->UBO, 0, 2 * sizeof(glm::mat4));
}

void ShaderManager::updateUniformBuffer(glm::mat4 view, glm::mat4 projection) const
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

		case ShaderType::HDRTOCUBEMAP:
			enumToShader[shader] = new Shader("shaders/cubemap.vert", "shaders/equirectangularToCubeMap.frag");
			break;

		case ShaderType::IRRADIANCE:
			enumToShader[shader] = new Shader("shaders/cubemap.vert", "shaders/irradiance.frag");
			break;

		case ShaderType::PREFILTER:
			enumToShader[shader] = new Shader("shaders/cubemap.vert", "shaders/prefilter.frag");
			break;

		case ShaderType::BRDF:
			enumToShader[shader] = new Shader("shaders/brdf.vert", "shaders/brdf.frag");
			break;

		case ShaderType::SOLID:
			enumToShader[shader] = new Shader("shaders/solid.vert", "shaders/solid.frag");
			break;

		case ShaderType::OUTLINE:
			enumToShader[shader] = new Shader("shaders/outline.vert", "shaders/outline.frag");
			break;

		case ShaderType::DEPTH:
			enumToShader[shader] = new Shader("shaders/depth.vert", "shaders/depth.frag");
			break;

		case ShaderType::DEPTH_CASCADED:
			enumToShader[shader] = new Shader("shaders/depth.vert", "shaders/depth.frag", "shaders/depth.geom");
			break;

		case ShaderType::GBUFFER:
			enumToShader[shader] = new Shader("shaders/gBuffer.vert", "shaders/gBuffer.frag");
			break;

		case ShaderType::SSAO:
			enumToShader[shader] = new Shader("shaders/ssao.vert", "shaders/ssao.frag");
			break;

		case ShaderType::SSAOBLUR:
			enumToShader[shader] = new Shader("shaders/ssaoBlur.vert", "shaders/ssaoBlur.frag");
			break;

		default:
			Logger::logError("ShaderManager::getShader called with an unhandled ShaderType!", "shaderManager.cpp");
			return nullptr;
	}

	unsigned int UBIShader = glGetUniformBlockIndex(enumToShader[shader]->getID(), "Matrices");

	if (UBIShader != GL_INVALID_INDEX)
		glUniformBlockBinding(enumToShader[shader]->getID(), UBIShader, 0);

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

void ShaderManager::setVertexShaderContent(const ShaderType shader, const std::string& content)
{
	if (enumToShader.count(shader) == 0)
		return;

	std::ofstream ofs(enumToShader[shader]->vertexPath, std::ofstream::out);

	ofs << content;

	ofs.close();
}

void ShaderManager::setFragmentShaderContent(const ShaderType shader, const std::string& content)
{
	if (enumToShader.count(shader) == 0)
		return;

	std::ofstream ofs(enumToShader[shader]->fragmentPath, std::ofstream::out);

	ofs << content;

	ofs.close();
}
