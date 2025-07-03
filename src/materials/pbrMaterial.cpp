#include "materials/pbrMaterial.hpp"
#include "renderer.hpp"
#include "logger.hpp"
#include "textureView.hpp"

const std::string PBRMaterial::USED_MAPS = "material.used_maps";

const std::string PBRMaterial::TEXTURE_ALBEDO = "material.texture_albedo";
const std::string PBRMaterial::TEXTURE_NORMAL = "material.texture_normal";
const std::string PBRMaterial::TEXTURE_METALLIC = "material.texture_metallic";
const std::string PBRMaterial::TEXTURE_ROUGHNESS = "material.texture_roughness";
const std::string PBRMaterial::TEXTURE_AO = "material.texture_ao";
const std::string PBRMaterial::TEXTURE_OPACITY = "material.texture_opacity";
const std::string PBRMaterial::TEXTURE_EMISSIVE = "material.texture_emissive";

const std::string PBRMaterial::ALBEDO = "material.albedo";
const std::string PBRMaterial::METALLIC = "material.metallic";
const std::string PBRMaterial::ROUGHNESS = "material.roughness";
const std::string PBRMaterial::AO = "material.ao";
const std::string PBRMaterial::OPACITY = "material.opacity";

const std::string PBRMaterial::IRRADIANCE_MAP = "irradianceMap";
const std::string PBRMaterial::PREFILTER_MAP = "prefilterMap";
const std::string PBRMaterial::BRDF_LUT = "brdfLUT";

const std::string PBRMaterial::SHADOW_MAP = "shadowMap";
const std::string PBRMaterial::LIGHT_SPACE_MATRICES[4] = { "lightSpaceMatrices[0]", "lightSpaceMatrices[1]", "lightSpaceMatrices[2]", "lightSpaceMatrices[3]"};
const std::string PBRMaterial::CASCADE_PLANE_DISTANCES[3] = { "cascadePlaneDistances[0]", "cascadePlaneDistances[1]", "cascadePlaneDistances[2]" };
const std::string PBRMaterial::CASCADE_COUNT = "cascadeCount";
const std::string PBRMaterial::FAR_PLANE = "farPlane";

const std::string PBRMaterial::SSAO_MAP = "ssaoMap";

Cubemap* PBRMaterial::irradianceMap = nullptr;
Cubemap* PBRMaterial::prefilterMap = nullptr;
Texture* PBRMaterial::brdfLut = nullptr;
std::shared_ptr<TextureView> PBRMaterial::shadowMap = nullptr;
std::unique_ptr<TextureView> PBRMaterial::ssaoMap = nullptr;
glm::mat4 PBRMaterial::lightSpaceMatrices[4]{};
float PBRMaterial::cascadePlaneDistances[3]{};
float PBRMaterial::farPlane = 0.0f;

PBRMaterial::PBRMaterial(Shader* shaderProgram) : Material(shaderProgram)
{
	this->PBRMaterial::init();
}

PBRMaterial::~PBRMaterial() = default;

void PBRMaterial::init()
{
	this->shaderProgram->use();
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_ALBEDO, 0);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_NORMAL, 1);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_METALLIC, 2);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_ROUGHNESS, 3);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_AO, 4);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_OPACITY, 5);
	this->shaderProgram->setInt(PBRMaterial::TEXTURE_EMISSIVE, 6);
	this->shaderProgram->setInt(PBRMaterial::IRRADIANCE_MAP, 7);
	this->shaderProgram->setInt(PBRMaterial::PREFILTER_MAP, 8);
	this->shaderProgram->setInt(PBRMaterial::BRDF_LUT, 9);
	this->shaderProgram->setInt(PBRMaterial::SHADOW_MAP, 10);
	this->shaderProgram->setInt(PBRMaterial::SSAO_MAP, 11);
}

void PBRMaterial::sendToShader()
{
	// Instead of sending 7 uniforms for telling which maps are toggled on/off, we send a single int where each bit corresponds to a texture
	uint8_t usedMaps = this->useAlbedoMap | this->useNormalMap | this->useMetallicMap | this->useRoughnessMap | this->useAoMap | this->useEmissiveMap;

	this->shaderProgram->setInt(PBRMaterial::USED_MAPS, usedMaps);

	if (this->useAlbedoMap)
	{
		glActiveTexture(GL_TEXTURE0);
		this->albedoTexture->bindTexture();
	}
	else
		this->shaderProgram->setVec3(PBRMaterial::ALBEDO, this->albedoColor);

	if (this->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE1);
		this->normalTexture->bindTexture();
	}

	if (this->useMetallicMap)
	{
		glActiveTexture(GL_TEXTURE2);
		this->metallicTexture->bindTexture();
	}
	else
		this->shaderProgram->setFloat(PBRMaterial::METALLIC, this->metallic);

	if (this->useRoughnessMap)
	{
		glActiveTexture(GL_TEXTURE3);
		this->roughnessTexture->bindTexture();
	}
	else
		this->shaderProgram->setFloat(PBRMaterial::ROUGHNESS, this->roughness);

	if (this->useAoMap)
	{
		glActiveTexture(GL_TEXTURE4);
		this->aoTexture->bindTexture();
	}
	else
		this->shaderProgram->setFloat(PBRMaterial::AO, this->ao);

	if (this->useOpacityMap)
	{
		glActiveTexture(GL_TEXTURE5);
		this->opacityTexture->bindTexture();
	}
	else
		this->shaderProgram->setFloat(PBRMaterial::OPACITY, this->opacity);

	if (this->useEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE6);
		this->emissiveTexture->bindTexture();
	}

	if (PBRMaterial::irradianceMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE7);
		PBRMaterial::irradianceMap->bind();
	}

	if (PBRMaterial::prefilterMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE8);
		PBRMaterial::prefilterMap->bind();
	}

	if (PBRMaterial::brdfLut != nullptr)
	{
		glActiveTexture(GL_TEXTURE9);
		PBRMaterial::brdfLut->bindTexture();
	}

	if (PBRMaterial::shadowMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE10);
		PBRMaterial::shadowMap->bindTexture();

		for (int i = 0; i < 4; i++)
			this->shaderProgram->setMat4(PBRMaterial::LIGHT_SPACE_MATRICES[i], PBRMaterial::lightSpaceMatrices[i]);

		for (int i = 0; i < 3; i++)
			this->shaderProgram->setFloat(PBRMaterial::CASCADE_PLANE_DISTANCES[i], PBRMaterial::cascadePlaneDistances[i]);

		this->shaderProgram->setInt(PBRMaterial::CASCADE_COUNT, Renderer::SHADOW_CASCADE_LEVELS);
		this->shaderProgram->setFloat(PBRMaterial::FAR_PLANE, PBRMaterial::farPlane);
	}

	if (PBRMaterial::ssaoMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE11);
		PBRMaterial::ssaoMap->bindTexture();
	}

	glActiveTexture(GL_TEXTURE0);
}

bool PBRMaterial::getIsTransparent()
{
	if (this->useAlbedoMap)
		return this->albedoTexture->format == GL_RGBA;
	
	return this->opacity != 1.0f;
}

void PBRMaterial::addTextures(const std::vector<std::shared_ptr<Texture>>& textures)
{
	for (const auto& texture : textures)
	{
		switch (texture->type)
		{
			case TextureType::TEXTURE_ALBEDO:
				this->addAlbedoMap(texture);
				break;

			case TextureType::TEXTURE_NORMAL:
				this->addNormalMap(texture);
				break;

			case TextureType::TEXTURE_METALLIC:
				this->addMetallicMap(texture);
				break;

			case TextureType::TEXTURE_ROUGHNESS:
				this->addRoughnessMap(texture);
				break;

			case TextureType::TEXTURE_AO:
				this->addAoMap(texture);
				break;

			case TextureType::TEXTURE_OPACITY:
				this->addOpacityMap(texture);
				break;

			case TextureType::TEXTURE_EMISSIVE:
				this->addEmissiveMap(texture);
				break;

			default:
				Logger::logError("Got incorrect texture type while adding textures to PBRMaterial!", "pbrMaterial.cpp");
				break;
		}
	}
}

void PBRMaterial::addAlbedoMap(const std::shared_ptr<Texture> &albedoTexture)
{
	this->albedoTexture = albedoTexture;
	this->useAlbedoMap = ALBEDO_MAP_FLAG;
}

void PBRMaterial::addNormalMap(const std::shared_ptr<Texture> &normalTexture)
{
	this->normalTexture = normalTexture;
	this->useNormalMap = NORMAL_MAP_FLAG;
}

void PBRMaterial::addMetallicMap(const std::shared_ptr<Texture> &metallicTexture)
{
	this->metallicTexture = metallicTexture;
	this->useMetallicMap = METALLIC_MAP_FLAG;
}

void PBRMaterial::addRoughnessMap(const std::shared_ptr<Texture> &roughnessTexture)
{
	this->roughnessTexture = roughnessTexture;
	this->useRoughnessMap = ROUGHNESS_MAP_FLAG;
}

void PBRMaterial::addAoMap(const std::shared_ptr<Texture> &aoTexture)
{
	this->aoTexture = aoTexture;
	this->useAoMap = AO_MAP_FLAG;
}

void PBRMaterial::addOpacityMap(const std::shared_ptr<Texture> &opacityTexture)
{
	this->opacityTexture = opacityTexture;
	this->useOpacityMap = OPACITY_MAP_FLAG;
}

void PBRMaterial::addEmissiveMap(const std::shared_ptr<Texture> &emissiveTexture)
{
	this->emissiveTexture = emissiveTexture;
	this->useEmissiveMap = EMISSIVE_MAP_FLAG;
}
