#pragma once

#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "cubemap.hpp"
#include "texture.hpp"

struct PBRMaterial : public virtual Material
{
public:
	static const std::string USED_MAPS;

	static const std::string TEXTURE_ALBEDO;
	static const std::string TEXTURE_NORMAL;
	static const std::string TEXTURE_METALLIC;
	static const std::string TEXTURE_ROUGHNESS;
	static const std::string TEXTURE_AO;
	static const std::string TEXTURE_OPACITY;
	static const std::string TEXTURE_EMISSIVE;

	static const std::string ALBEDO;
	static const std::string METALLIC;
	static const std::string ROUGHNESS;
	static const std::string AO;
	static const std::string OPACITY;

	static const std::string IRRADIANCE_MAP;
	static const std::string PREFILTER_MAP;
	static const std::string BRDF_LUT;

	static const std::string SHADOW_MAP;
	static const std::string LIGHT_SPACE_MATRIX;

	static const std::string SSAO_MAP;

	enum UsedMaps
	{
		ALBEDO_MAP_FLAG = 1,
		NORMAL_MAP_FLAG = 2,
		METALLIC_MAP_FLAG = 4,
		ROUGHNESS_MAP_FLAG = 8,
		AO_MAP_FLAG = 16,
		OPACITY_MAP_FLAG = 32,
		EMISSIVE_MAP_FLAG = 64,
	};

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
	static Texture* brdfLut;

	/// <summary>
	/// The shadow map for shadow calculations
	/// </summary>
	static std::shared_ptr<Texture> shadowMap;

	/// <summary>
	/// The SSAO map for applying screen space ambient occlusion
	/// </summary>
	static std::shared_ptr<Texture> ssaoMap;

	/// <summary>
	/// The light space matrix for shadow calculations
	/// </summary>
	static glm::mat4 lightSpaceMatrix;

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

	int useAlbedoMap = 0;
	int useNormalMap = 0;
	int useMetallicMap = 0;
	int useRoughnessMap = 0;
	int useAoMap = 0;
	int useOpacityMap = 0;
	int useEmissiveMap = 0;

	PBRMaterial(Shader* shaderProgram);
	~PBRMaterial() override;

	void init() override;
	void sendToShader() override;
	bool getIsTransparent() override;
	void addTextures(std::vector<std::shared_ptr<Texture>> textures) override;

	void addAlbedoMap(std::shared_ptr<Texture> albedoTexture);
	void addNormalMap(std::shared_ptr<Texture> normalTexture);
	void addMetallicMap(std::shared_ptr<Texture> metallicTexture);
	void addRoughnessMap(std::shared_ptr<Texture> roughnessTexture);
	void addAoMap(std::shared_ptr<Texture> aoTexture);
	void addOpacityMap(std::shared_ptr<Texture> opacityTexture);
	void addEmissiveMap(std::shared_ptr<Texture> emissiveTexture);
};