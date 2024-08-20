#include "materials/pbrMaterial.hpp"
#include "main.hpp"

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
const std::string PBRMaterial::LIGHT_SPACE_MATRIX = "lightSpaceMatrix";

Cubemap* PBRMaterial::irradianceMap = nullptr;
Cubemap* PBRMaterial::prefilterMap = nullptr;
std::shared_ptr<Texture> PBRMaterial::brdfLut = nullptr;
std::shared_ptr<Texture> PBRMaterial::shadowMap = nullptr;
glm::mat4 PBRMaterial::lightSpaceMatrix = glm::mat4(1.0f);

PBRMaterial::PBRMaterial(Shader* shaderProgram) : Material(shaderProgram)
{
	this->init();
}

PBRMaterial::~PBRMaterial()
{

}

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
		this->roughnessTexture->bindTexture();
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

	if (this->irradianceMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE7);
		this->irradianceMap->bind();
	}

	if (this->prefilterMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE8);
		this->prefilterMap->bind();
	}

	if (this->brdfLut != nullptr)
	{
		glActiveTexture(GL_TEXTURE9);
		this->brdfLut->bindTexture();
	}

	if (this->shadowMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE10);
		this->shadowMap->bindTexture();
		this->shaderProgram->setMat4(PBRMaterial::LIGHT_SPACE_MATRIX, this->lightSpaceMatrix);
	}

	glActiveTexture(GL_TEXTURE0);
}

void PBRMaterial::addTextures(std::vector<std::shared_ptr<Texture>> textures)
{
	for (int i = 0; i < textures.size(); i++)
	{
		switch (textures[i]->type)
		{
			case TextureType::TEXTURE_ALBEDO:
				this->addAlbedoMap(textures[i]);
				break;

			case TextureType::TEXTURE_NORMAL:
				this->addNormalMap(textures[i]);
				break;

			case TextureType::TEXTURE_METALLIC:
				this->addMetallicMap(textures[i]);
				break;

			case TextureType::TEXTURE_ROUGHNESS:
				this->addRoughnessMap(textures[i]);
				break;

			case TextureType::TEXTURE_AO:
				this->addAoMap(textures[i]);
				break;

			case TextureType::TEXTURE_OPACITY:
				this->addOpacityMap(textures[i]);
				break;

			case TextureType::TEXTURE_EMISSIVE:
				this->addEmissiveMap(textures[i]);
				break;
		}
	}
}

MaterialType PBRMaterial::getType()
{
	return MaterialType::PBRMaterial;
}

void PBRMaterial::addAlbedoMap(std::shared_ptr<Texture> albedoTexture)
{
	this->albedoTexture = albedoTexture;
	this->useAlbedoMap = ALBEDO_MAP_FLAG;
}

void PBRMaterial::addNormalMap(std::shared_ptr<Texture> normalTexture)
{
	this->normalTexture = normalTexture;
	this->useNormalMap = NORMAL_MAP_FLAG;
}

void PBRMaterial::addMetallicMap(std::shared_ptr<Texture> metallicTexture)
{
	this->metallicTexture = metallicTexture;
	this->useMetallicMap = METALLIC_MAP_FLAG;
}

void PBRMaterial::addRoughnessMap(std::shared_ptr<Texture> roughnessTexture)
{
	this->roughnessTexture = roughnessTexture;
	this->useRoughnessMap = ROUGHNESS_MAP_FLAG;
}

void PBRMaterial::addAoMap(std::shared_ptr<Texture> aoTexture)
{
	this->aoTexture = aoTexture;
	this->useAoMap = AO_MAP_FLAG;
}

void PBRMaterial::addOpacityMap(std::shared_ptr<Texture> opacityTexture)
{
	this->opacityTexture = opacityTexture;
	this->useOpacityMap = OPACITY_MAP_FLAG;
}

void PBRMaterial::addEmissiveMap(std::shared_ptr<Texture> emissiveTexture)
{
	this->emissiveTexture = emissiveTexture;
	this->useEmissiveMap = EMISSIVE_MAP_FLAG;
}
