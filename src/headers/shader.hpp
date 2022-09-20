#ifndef SHADER_HPP
#define SHADER_HPP

// A class used to easily compile shaders from a given source and setup their data to be used by OpenGL
// Also contains helper methods to easily modify the shader's uniforms
class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
	
	// Various methods to set uniforms for the shader
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& value) const;
	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;

private:
	void checkCompileErrors(GLuint shader, std::string type);
};

#endif