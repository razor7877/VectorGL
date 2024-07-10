#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>

#include "texture.hpp"

/// <summary>
/// Contains all the material data for a mesh
/// </summary>
struct Material
{
public:
	Material();
	Material(Texture* texture);
	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, Texture* texture);

	void addDiffuseMap(Texture* diffuseTexture);
	void addSpecularMap(Texture* specularTexture);
	void addNormalMap(Texture* normalTexture);
	void addHeightMap(Texture* heightTexture);

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
	Texture* diffuseTexture = nullptr;

	/// <summary>
	/// The specular texture of the object, if it has one
	/// </summary>
	Texture* specularTexture = nullptr;

	/// <summary>
	/// The normal texture of the object, if it has one
	/// </summary>
	Texture* normalTexture = nullptr;

	/// <summary>
	/// The height texture of the object, if it has one
	/// </summary>
	Texture* heightTexture = nullptr;

	bool useDiffuseMap = false;
	bool useSpecularMap = false;
	bool useNormalMap = false;
	bool useHeightMap = false;
};

#endif