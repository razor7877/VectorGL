#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <utilities/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "logger.hpp"

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;

	this->compileShader();
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath)
{
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;
	this->geometryPath = geometryPath;

	this->compileShader();
}

Shader::~Shader()
{
	Logger::logDebug(std::string("Deleting shader with ID ") + std::to_string(this->ID), "shader.cpp");
	glDeleteProgram(this->ID);
}

Shader* Shader::use()
{
	glUseProgram(this->ID);
	return this;
}

bool Shader::compileShader()
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		std::stringstream vShaderStream;
		vShaderFile.open(this->vertexPath);
		vShaderStream << vShaderFile.rdbuf();
		vShaderFile.close();
		vertexCode = vShaderStream.str();

		std::stringstream fShaderStream;
		fShaderFile.open(this->fragmentPath);
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();

		if (!this->geometryPath.empty())
		{
			std::stringstream gShaderStream;
			gShaderFile.open(this->geometryPath);
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		Logger::logError("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ", "shader.cpp");
		return false;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex{};
	unsigned int fragment{};
	unsigned int geometry{};

	int success{};
	char infoLog[512]{};

	// Create and compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		Logger::logError(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog, "shader.cpp");
		return false;
	}

	// Create and compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		Logger::logError(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + infoLog, "shader.cpp");
		return false;
	}

	// If present, create and compile geometry shader
	if (!this->geometryPath.empty())
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			Logger::logError(std::string("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n") + infoLog, "shader.cpp");
			return false;
		}
	}

	GLuint newID = glCreateProgram();
	glAttachShader(newID, vertex);
	glAttachShader(newID, fragment);
	if (!this->geometryPath.empty())
		glAttachShader(newID, geometry);

	glLinkProgram(newID);

	glGetProgramiv(newID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(newID, 512, NULL, infoLog);
		Logger::logError(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + infoLog, "shader.cpp");
		return false;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (!this->geometryPath.empty())
		glDeleteShader(geometry);

	// Delete program if we had an old one (when recompiling)
	if (this->ID != 0)
		glDeleteProgram(this->ID);

	// We can replace the new ID if everything succeeded
	this->ID = newID;

	Logger::logInfo("Successfully compiled shader!", "shader.cpp");
	this->wasRecompiled = true;

	return true;
}

GLuint Shader::getID()
{
	return this->ID;
}

Shader* Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(this->getUniformLocation(name), static_cast<int>(value));
	return this;
}

Shader* Shader::setInt(const std::string& name, int value)
{
	glUniform1i(this->getUniformLocation(name), value);
	return this;
}

Shader* Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(this->getUniformLocation(name), value);
	return this;
}

Shader* Shader::setVec2(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(this->getUniformLocation(name), 1, &value[0]);
	return this;
}

Shader* Shader::setVec2(const std::string& name, float x, float y)
{
	glUniform2f(this->getUniformLocation(name), x, y);
	return this;
}

Shader* Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	glUniform3fv(this->getUniformLocation(name), 1, &value[0]);
	return this;
}

Shader* Shader::setVec3(const std::string& name, float x, float y, float z)
{
	glUniform3f(this->getUniformLocation(name), x, y, z);
	return this;
}

Shader* Shader::setVec4(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(this->getUniformLocation(name), 1, &value[0]);
	return this;
}

Shader* Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(this->getUniformLocation(name), x, y, z, w);
	return this;
}

Shader* Shader::setMat2(const std::string& name, const glm::mat2& value)
{
	glUniformMatrix2fv(this->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	return this;
}

Shader* Shader::setMat3(const std::string& name, const glm::mat3& value)
{
	glUniformMatrix3fv(this->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	return this;
}

Shader* Shader::setMat4(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	return this;
}

GLuint Shader::getUniformLocation(const std::string& uniformName)
{
	if (this->locationCache.count(uniformName) == 0)
		this->locationCache[uniformName] = glGetUniformLocation(this->ID, uniformName.c_str());

	return this->locationCache[uniformName];
}