#include "materials/pbrMaterial.hpp"

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
