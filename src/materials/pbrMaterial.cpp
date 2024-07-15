#include "materials/pbrMaterial.hpp"
#include "main.hpp"

Cubemap* PBRMaterial::irradianceMap = nullptr;
Cubemap* PBRMaterial::prefilterMap = nullptr;
Texture* PBRMaterial::brdfLut = nullptr;

PBRMaterial::PBRMaterial()
{

}

void PBRMaterial::sendToShader(Shader* shaderProgram)
{
	// Send all data relevant to textures
	shaderProgram->setInt("material.use_albedo_map", this->useAlbedoMap);
	shaderProgram->setInt("material.use_normal_map", this->useNormalMap);
	shaderProgram->setInt("material.use_metallic_map", this->useMetallicMap);
	shaderProgram->setInt("material.use_roughness_map", this->useRoughnessMap);
	shaderProgram->setInt("material.use_ao_map", this->useAoMap);
	shaderProgram->setInt("material.use_emissive_map", this->useEmissiveMap);

	if (this->useAlbedoMap)
	{
		glActiveTexture(GL_TEXTURE0);
		shaderProgram->setInt("material.texture_albedo", 0);
		this->albedoTexture->bindTexture();
	}
	else
		shaderProgram->setVec3("material.albedo", this->albedoColor);

	if (this->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE1);
		shaderProgram->setInt("material.texture_normal", 1);
		this->normalTexture->bindTexture();
	}

	if (this->useMetallicMap)
	{
		glActiveTexture(GL_TEXTURE2);
		shaderProgram->setInt("material.texture_metallic", 2);
		this->metallicTexture->bindTexture();
	}
	else
		shaderProgram->setFloat("material.metallic", this->metallic);
	
	if (this->useRoughnessMap)
	{
		glActiveTexture(GL_TEXTURE3);
		shaderProgram->setInt("material.texture_roughness", 3);
		this->roughnessTexture->bindTexture();
	}
	else
		shaderProgram->setFloat("material.roughness", this->roughness);

	if (this->useAoMap)
	{
		glActiveTexture(GL_TEXTURE4);
		shaderProgram->setInt("material.texture_ao", 4);
		this->roughnessTexture->bindTexture();
	}
	else
		shaderProgram->setFloat("material.ao", this->ao);

	if (this->useOpacityMap)
	{
		glActiveTexture(GL_TEXTURE5);
		shaderProgram->setInt("material.texture_opacity", 5);
		this->opacityTexture->bindTexture();
	}
	else
		shaderProgram->setFloat("material.opacity", this->opacity);

	if (this->useEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE6);
		shaderProgram->setInt("material.texture_emissive", 6);
		this->emissiveTexture->bindTexture();
	}

	if (this->irradianceMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE7);
		shaderProgram->setInt("irradianceMap", 7);
		this->irradianceMap->bind();
	}

	if (this->prefilterMap != nullptr)
	{
		glActiveTexture(GL_TEXTURE8);
		shaderProgram->setInt("prefilterMap", 8);
		this->prefilterMap->bind();
	}

	if (this->brdfLut != nullptr)
	{
		glActiveTexture(GL_TEXTURE9);
		shaderProgram->setInt("brdfLUT", 9);
		this->brdfLut->bindTexture();
	}
}

void PBRMaterial::addAlbedoMap(Texture* albedoTexture)
{
	this->albedoTexture = albedoTexture;
	this->useAlbedoMap = true;
}

void PBRMaterial::addNormalMap(Texture* normalTexture)
{
	this->normalTexture = normalTexture;
	this->useNormalMap = true;
}

void PBRMaterial::addMetallicMap(Texture* metallicTexture)
{
	this->metallicTexture = metallicTexture;
	this->useMetallicMap = true;
}

void PBRMaterial::addRoughnessMap(Texture* roughnessTexture)
{
	this->roughnessTexture = roughnessTexture;
	this->useRoughnessMap = true;
}

void PBRMaterial::addAoMap(Texture* aoTexture)
{
	this->aoTexture = aoTexture;
	this->useAoMap = true;
}

void PBRMaterial::addOpacityMap(Texture* opacityTexture)
{
	this->opacityTexture = opacityTexture;
	this->useOpacityMap = true;
}

void PBRMaterial::addEmissiveMap(Texture* emissiveTexture)
{
	this->emissiveTexture = emissiveTexture;
	this->useEmissiveMap = true;
}
