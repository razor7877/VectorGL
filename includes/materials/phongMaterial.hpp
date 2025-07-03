#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "texture.hpp"
#include "shader.hpp"

/// <summary>
/// Contains all the material data for a mesh
/// </summary>
struct PhongMaterial : public virtual Material
{
public:
	static const std::string AMBIENT_COLOR;
	static const std::string DIFFUSE_COLOR;
	static const std::string SPECULAR_COLOR;
	static const std::string SHININESS;

	static const std::string USE_DIFFUSE_MAP;
	static const std::string USE_SPECULAR_MAP;
	static const std::string USE_NORMAL_MAP;
	static const std::string USE_HEIGHT_MAP;
	static const std::string USE_EMISSIVE_MAP;

	static const std::string TEXTURE_DIFFUSE;
	static const std::string TEXTURE_SPECULAR;
	static const std::string TEXTURE_NORMAL;
	static const std::string TEXTURE_HEIGHT;
	static const std::string TEXTURE_EMISSIVE;

	explicit PhongMaterial(Shader* shaderProgram);
	PhongMaterial(Shader* shaderProgram, std::shared_ptr<Texture> texture);
	PhongMaterial(Shader* shaderProgram, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, std::shared_ptr<Texture> texture);
	~PhongMaterial() override;

	void init() override;

	/// <summary>
	/// Sends the material data to a shader, the method assumes the shader is already in use
	/// </summary>
	/// <param name="shaderProgram">The shader to send the data to</param>
	void sendToShader() override;

	bool getIsTransparent() override;

	void addTextures(const std::vector<std::shared_ptr<Texture>>& textures) override;

	void addDiffuseMap(std::shared_ptr<Texture> diffuseTexture);
	void addSpecularMap(std::shared_ptr<Texture> specularTexture);
	void addNormalMap(std::shared_ptr<Texture> normalTexture);
	void addHeightMap(std::shared_ptr<Texture> heightTexture);
	void addEmissiveMap(std::shared_ptr<Texture> emissiveTexture);

	/// <summary>
	/// The ambient color of the object
	/// </summary>
	glm::vec3 ambientColor = glm::vec3(1.0f);

	/// <summary>
	/// The diffuse color of the object, used in case it doesn't have a diffuse texture
	/// </summary>
	glm::vec3 diffuseColor = glm::vec3(1.0f);

	/// <summary>
	/// The specular color of the object, used in case it doesn't have a specular texture
	/// </summary>
	glm::vec3 specularColor = glm::vec3(1.0f);

	float shininess = 1.0f;

	/// <summary>
	/// The diffuse texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> diffuseTexture = nullptr;

	/// <summary>
	/// The specular texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> specularTexture = nullptr;

	/// <summary>
	/// The normal texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> normalTexture = nullptr;

	/// <summary>
	/// The height texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> heightTexture = nullptr;

	/// <summary>
	/// The emissive texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> emissiveTexture = nullptr;

	bool useDiffuseMap = false;
	bool useSpecularMap = false;
	bool useNormalMap = false;
	bool useHeightMap = false;
	bool useEmissiveMap = false;
};

#endif