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
	PhongMaterial();
	PhongMaterial(std::shared_ptr<Texture> texture);
	PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, std::shared_ptr<Texture> texture);
	~PhongMaterial() override;

	/// <summary>
	/// Sends the material data to a shader, the method assumes the shader is already in use
	/// </summary>
	/// <param name="shaderProgram">The shader to send the data to</param>
	void sendToShader(Shader* shaderProgram) override;

	void addTextures(std::vector<std::shared_ptr<Texture>> textures) override;

	void addDiffuseMap(std::shared_ptr<Texture> diffuseTexture);
	void addSpecularMap(std::shared_ptr<Texture> specularTexture);
	void addNormalMap(std::shared_ptr<Texture> normalTexture);
	void addHeightMap(std::shared_ptr<Texture> heightTexture);

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

	bool useDiffuseMap = false;
	bool useSpecularMap = false;
	bool useNormalMap = false;
	bool useHeightMap = false;
};

#endif