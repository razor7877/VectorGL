#include "materials/phongMaterial.hpp"

PhongMaterial::PhongMaterial()
{

}

PhongMaterial::PhongMaterial(Texture* texture)
{
	this->addDiffuseMap(texture);
}

PhongMaterial::PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, Texture* texture)
{
	this->ambientColor = ambient;
	this->diffuseColor = diffuse;
	this->specularColor = specular;
	this->shininess = shininess;
	this->diffuseTexture = texture;
}

void PhongMaterial::sendToShader(Shader* shaderProgram)
{
	// Send all data relevant to textures
	shaderProgram->setInt("material.use_diffuse_map", this->useDiffuseMap);
	shaderProgram->setInt("material.use_specular_map", this->useSpecularMap);
	shaderProgram->setInt("material.use_normal_map", this->useNormalMap);
	shaderProgram->setInt("material.use_height_map", this->useHeightMap);

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

	// Send the model matrix & material data
	shaderProgram
		->setVec3("material.ambient", this->ambientColor)
		->setVec3("material.diffuse", this->diffuseColor)
		->setVec3("material.specular", this->specularColor)
		->setFloat("material.shininess", this->shininess);
}

void PhongMaterial::addDiffuseMap(Texture* diffuseTexture)
{
	this->useDiffuseMap = true;
	this->diffuseTexture = diffuseTexture;
}

void PhongMaterial::addSpecularMap(Texture* specularTexture)
{
	this->useSpecularMap = true;
	this->specularTexture = specularTexture;
}

void PhongMaterial::addNormalMap(Texture* normalTexture)
{
	this->useNormalMap = true;
	this->normalTexture = normalTexture;
}

void PhongMaterial::addHeightMap(Texture* heightTexture)
{
	this->useHeightMap = true;
	this->heightTexture = heightTexture;
}
