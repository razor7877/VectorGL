#pragma once

#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "texture.hpp"

struct PBRMaterial : public virtual Material
{
public:
	PBRMaterial();

	void sendToShader(Shader* shaderProgram) override;

	void addAlbedoMap(Texture* albedoTexture);
	void addNormalMap(Texture* normalTexture);
	void addMetallicMap(Texture* metallicTexture);
	void addRoughnessMap(Texture* roughnessTexture);
	void addAoMap(Texture* aoTexture);

	/// <summary>
	/// The albedo color of the object, used in case it doesn't have an albedo texture
	/// </summary>
	glm::vec3 albedoColor = glm::vec3(1.0f);

	/// <summary>
	/// The metallic value of the object, used in case it doesn't have a metallic texture
	/// </summary>
	float metallic = 0.0f;

	/// <summary>
	/// The roughness value of the object, used in case it doesn't have a roughness texture
	/// </summary>
	float roughness = 0.5f;

	/// <summary>
	/// The ambient occlusion value of the object, used in case it doesn't have an AO texture
	/// </summary>
	float ao = 1.0f;

	/// <summary>
	/// The opacity value of the object
	/// </summary>
	float opacity = 1.0f;

	/// <summary>
	/// The albedo texture of the object responsible for the color, if it has one
	/// </summary>
	Texture* albedoTexture = nullptr;

	/// <summary>
	/// The normal texture of the object for lighting calculations, if it has one
	/// </summary>
	Texture* normalTexture = nullptr;

	/// <summary>
	/// The metallic texture of the object, if it has one
	/// </summary>
	Texture* metallicTexture = nullptr;

	/// <summary>
	/// The roughness texture of the object, if it has one
	/// </summary>
	Texture* roughnessTexture = nullptr;

	/// <summary>
	/// The ambient occlusion texture of the object, if it has one
	/// </summary>
	Texture* aoTexture = nullptr;

	bool useAlbedoMap = false;
	bool useNormalMap = false;
	bool useMetallicMap = false;
	bool useRoughnessMap = false;
	bool useAoMap = false;
};