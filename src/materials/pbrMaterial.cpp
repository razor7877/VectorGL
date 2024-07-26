#include "materials/pbrMaterial.hpp"
#include "main.hpp"

const std::string PBRMaterial::USE_ALBEDO_MAP = "material.use_albedo_map";
const std::string PBRMaterial::USE_NORMAL_MAP = "material.use_normal_map";
const std::string PBRMaterial::USE_METALLIC_MAP = "material.use_metallic_map";
const std::string PBRMaterial::USE_ROUGHNESS_MAP = "material.use_roughness_map";
const std::string PBRMaterial::USE_AO_MAP = "material.use_ao_map";
const std::string PBRMaterial::USE_EMISSIVE_MAP = "material.use_emissive_map";

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

Cubemap* PBRMaterial::irradianceMap = nullptr;
Cubemap* PBRMaterial::prefilterMap = nullptr;
std::shared_ptr<Texture> PBRMaterial::brdfLut = nullptr;

PBRMaterial::PBRMaterial(Shader* shaderProgram) : Material(shaderProgram)
{

}

PBRMaterial::~PBRMaterial()
{

}

void PBRMaterial::sendToShader()
{
	// Send all data relevant to textures
	this->shaderProgram->setInt(PBRMaterial::USE_ALBEDO_MAP, this->useAlbedoMap);
	this->shaderProgram->setInt(PBRMaterial::USE_NORMAL_MAP, this->useNormalMap);
	this->shaderProgram->setInt(PBRMaterial::USE_METALLIC_MAP, this->useMetallicMap);
	this->shaderProgram->setInt(PBRMaterial::USE_ROUGHNESS_MAP, this->useRoughnessMap);
	this->shaderProgram->setInt(PBRMaterial::USE_AO_MAP, this->useAoMap);
	this->shaderProgram->setInt(PBRMaterial::USE_EMISSIVE_MAP, this->useEmissiveMap);

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
	this->useAlbedoMap = true;
}

void PBRMaterial::addNormalMap(std::shared_ptr<Texture> normalTexture)
{
	this->normalTexture = normalTexture;
	this->useNormalMap = true;
}

void PBRMaterial::addMetallicMap(std::shared_ptr<Texture> metallicTexture)
{
	this->metallicTexture = metallicTexture;
	this->useMetallicMap = true;
}

void PBRMaterial::addRoughnessMap(std::shared_ptr<Texture> roughnessTexture)
{
	this->roughnessTexture = roughnessTexture;
	this->useRoughnessMap = true;
}

void PBRMaterial::addAoMap(std::shared_ptr<Texture> aoTexture)
{
	this->aoTexture = aoTexture;
	this->useAoMap = true;
}

void PBRMaterial::addOpacityMap(std::shared_ptr<Texture> opacityTexture)
{
	this->opacityTexture = opacityTexture;
	this->useOpacityMap = true;
}

void PBRMaterial::addEmissiveMap(std::shared_ptr<Texture> emissiveTexture)
{
	this->emissiveTexture = emissiveTexture;
	this->useEmissiveMap = true;
}
