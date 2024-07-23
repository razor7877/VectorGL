#pragma once

#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "cubemap.hpp"
#include "texture.hpp"

struct PBRMaterial : public virtual Material
{
public:
	/// <summary>
	/// The irradiance texture of the sky for diffuse IBL
	/// </summary>
	static Cubemap* irradianceMap;

	/// <summary>
	/// The prefiltered map of the sky for specular IBL
	/// </summary>
	static Cubemap* prefilterMap;

	/// <summary>
	/// The BRDF look up table for specular IBL
	/// </summary>
	static std::shared_ptr<Texture> brdfLut;

	/// <summary>
	/// The albedo color of the object, used in case it doesn't have an albedo texture
	/// </summary>
	glm::vec3 albedoColor = glm::vec3(1.0f);

	/// <summary>
	/// The metallic value of the object, used in case it doesn't have a metallic texture
	/// </summary>
	float metallic = 0.025f;

	/// <summary>
	/// The roughness value of the object, used in case it doesn't have a roughness texture
	/// </summary>
	float roughness = 0.8f;

	/// <summary>
	/// The ambient occlusion value of the object, used in case it doesn't have an AO texture
	/// </summary>
	float ao = 1.0f;

	/// <summary>
	/// The opacity value of the object, used in case it doesn't have an opacity texture
	/// </summary>
	float opacity = 1.0f;

	/// <summary>
	/// The albedo texture of the object responsible for the color, if it has one
	/// </summary>
	std::shared_ptr<Texture> albedoTexture = nullptr;

	/// <summary>
	/// The normal texture of the object for lighting calculations, if it has one
	/// </summary>
	std::shared_ptr<Texture> normalTexture = nullptr;

	/// <summary>
	/// The metallic texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> metallicTexture = nullptr;

	/// <summary>
	/// The roughness texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> roughnessTexture = nullptr;

	/// <summary>
	/// The ambient occlusion texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> aoTexture = nullptr;

	/// <summary>
	/// The opacity texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> opacityTexture = nullptr;

	/// <summary>
	/// The emissive texture of the object, if it has one
	/// </summary>
	std::shared_ptr<Texture> emissiveTexture = nullptr;

	bool useAlbedoMap = false;
	bool useNormalMap = false;
	bool useMetallicMap = false;
	bool useRoughnessMap = false;
	bool useAoMap = false;
	bool useOpacityMap = false;
	bool useEmissiveMap = false;

	PBRMaterial();
	~PBRMaterial() override;

	void sendToShader(Shader* shaderProgram) override;
	void addTextures(std::vector<std::shared_ptr<Texture>> textures) override;

	void addAlbedoMap(std::shared_ptr<Texture> albedoTexture);
	void addNormalMap(std::shared_ptr<Texture> normalTexture);
	void addMetallicMap(std::shared_ptr<Texture> metallicTexture);
	void addRoughnessMap(std::shared_ptr<Texture> roughnessTexture);
	void addAoMap(std::shared_ptr<Texture> aoTexture);
	void addOpacityMap(std::shared_ptr<Texture> opacityTexture);
	void addEmissiveMap(std::shared_ptr<Texture> emissiveTexture);
};