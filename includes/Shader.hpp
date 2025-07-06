#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

#include <utilities/glad.h>
#include <glm/glm.hpp>

// A class used to easily compile shaders from a given source and setup their data to be used by OpenGL
// Also contains helper methods to easily modify the shader's uniforms
class Shader
{
public:
	/// <summary>
	/// The file system path to the vertex shader
	/// </summary>
	std::string vertexPath;

	/// <summary>
	/// The file system path to the fragment shader
	/// </summary>
	std::string fragmentPath;

	/// <summary>
	/// The file system path to the geometry shader, if there is one
	/// </summary>
	std::string geometryPath;

	/// <summary>
	/// Whether the shader has been recompiled
	/// </summary>
	bool wasRecompiled = false;

	/// <summary>
	/// Creates a new OpenGL shader
	/// </summary>
	/// <param name="vertexPath">A path to the vertex shader file</param>
	/// <param name="fragmentPath">A path to the fragment shader file</param>
	Shader(const std::string &vertexPath, const std::string &fragmentPath);

	/// <summary>
	/// Creates a new OpenGL shader
	/// </summary>
	/// <param name="vertexPath">A path to the vertex shader file</param>
	/// <param name="fragmentPath">A path to the fragment shader file</param>
	/// <param name="geometryPath">A path to the geometry shader file</param>
	Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath);

	/// <summary>
	/// Deletes the OpenGL shader
	/// </summary>
	~Shader();

	/// <summary>
	/// Binds the shader
	/// </summary>
	/// <returns></returns>
	Shader* use();

	/// <summary>
	/// Compiles the shader, can be called after modifying the files to recompile the shader during runtime
	/// </summary>
	/// <returns>True if the shader was successfully compiled, false otherwise</returns>
	bool compileShader();

	GLuint getID() const;
	
	Shader* setBool(const std::string& name, bool value);
	Shader* setInt(const std::string& name, int value);
	Shader* setFloat(const std::string& name, float value);
	Shader* setVec2(const std::string& name, const glm::vec2& value);
	Shader* setVec2(const std::string& name, float x, float y);
	Shader* setVec3(const std::string& name, const glm::vec3& value);
	Shader* setVec3(const std::string& name, float x, float y, float z);
	Shader* setVec4(const std::string& name, const glm::vec4& value);
	Shader* setVec4(const std::string& name, float x, float y, float z, float w);
	Shader* setMat2(const std::string& name, const glm::mat2& value);
	Shader* setMat3(const std::string& name, const glm::mat3& value);
	Shader* setMat4(const std::string& name, const glm::mat4& value);

private:
	/// <summary>
	/// The OpenGL handle for this program
	/// </summary>
	GLuint ID = 0;

	/// <summary>
	/// The shader keeps a cache of the uniform locations to improve runtime performance
	/// </summary>
	std::map<std::string, GLuint> locationCache;

	GLuint getUniformLocation(const std::string& uniformName);
};

#endif