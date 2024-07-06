#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

// A class used to easily compile shaders from a given source and setup their data to be used by OpenGL
// Also contains helper methods to easily modify the shader's uniforms
class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	Shader* use();
	
	// Various methods to set uniforms for the shader
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
	void checkCompileErrors(GLuint shader, std::string type);
};

#endif