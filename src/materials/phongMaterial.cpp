#include "materials/phongMaterial.hpp"

PhongMaterial::PhongMaterial()
{

}

PhongMaterial::PhongMaterial(std::shared_ptr<Texture> texture)
{
	this->addDiffuseMap(texture);
}

PhongMaterial::PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, std::shared_ptr<Texture> texture)
{
	this->ambientColor = ambient;
	this->diffuseColor = diffuse;
	this->specularColor = specular;
	this->shininess = shininess;
	this->diffuseTexture = texture;
}

PhongMaterial::~PhongMaterial()
{

}

void PhongMaterial::sendToShader(Shader* shaderProgram)
{
	shaderProgram
		->setVec3("material.ambient", this->ambientColor)
		->setVec3("material.diffuse", this->diffuseColor)
		->setVec3("material.specular", this->specularColor)
		->setFloat("material.shininess", this->shininess);

	// Send all data relevant to textures
	shaderProgram->setInt("material.use_diffuse_map", this->useDiffuseMap);
	shaderProgram->setInt("material.use_specular_map", this->useSpecularMap);
	shaderProgram->setInt("material.use_normal_map", this->useNormalMap);
	shaderProgram->setInt("material.use_height_map", this->useHeightMap);
	shaderProgram->setInt("material.use_emissive_map", this->useEmissiveMap);

	// Bind all the textures needed
	if (this->useDiffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		shaderProgram->setInt("material.texture_diffuse", 0);
		this->diffuseTexture->bindTexture();
	}
	else
		shaderProgram->setVec3("material.diffuse_color", this->diffuseColor);

	if (this->useSpecularMap)
	{
		glActiveTexture(GL_TEXTURE1);
		shaderProgram->setInt("material.texture_specular", 1);
		this->specularTexture->bindTexture();
	}

	if (this->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE2);
		shaderProgram->setInt("material.texture_normal", 2);
		this->normalTexture->bindTexture();
	}

	if (this->useHeightMap)
	{
		glActiveTexture(GL_TEXTURE3);
		shaderProgram->setInt("material.texture_height", 3);
		this->heightTexture->bindTexture();
	}

	if (this->useEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE4);
		shaderProgram->setInt("material.texture_emissive", 4);
		this->emissiveTexture->bindTexture();
	}
}

void PhongMaterial::addTextures(std::vector<std::shared_ptr<Texture>> textures)
{
	// We sort the textures from the vector into their own members in the mesh for easier use later
	for (int i = 0; i < textures.size(); i++)
	{
		switch (textures[i]->type)
		{
			case TextureType::TEXTURE_DIFFUSE:
				this->addDiffuseMap(textures[i]);
				break;

			case TextureType::TEXTURE_SPECULAR:
				this->addSpecularMap(textures[i]);
				break;

			case TextureType::TEXTURE_NORMAL:
				this->addNormalMap(textures[i]);
				break;

			case TextureType::TEXTURE_HEIGHT:
				this->addHeightMap(textures[i]);
				break;

			case TextureType::TEXTURE_EMISSIVE:
				this->addEmissiveMap(textures[i]);
		}
	}
}

void PhongMaterial::addDiffuseMap(std::shared_ptr<Texture> diffuseTexture)
{
	this->useDiffuseMap = true;
	this->diffuseTexture = diffuseTexture;
}

void PhongMaterial::addSpecularMap(std::shared_ptr<Texture> specularTexture)
{
	this->useSpecularMap = true;
	this->specularTexture = specularTexture;
}

void PhongMaterial::addNormalMap(std::shared_ptr<Texture> normalTexture)
{
	this->useNormalMap = true;
	this->normalTexture = normalTexture;
}

void PhongMaterial::addHeightMap(std::shared_ptr<Texture> heightTexture)
{
	this->useHeightMap = true;
	this->heightTexture = heightTexture;
}

void PhongMaterial::addEmissiveMap(std::shared_ptr<Texture> emissiveTexture)
{
	this->useEmissiveMap = true;
	this->emissiveTexture = emissiveTexture;
}
