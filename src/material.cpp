#include "material.hpp"

Material::Material()
{

}

Material::Material(Texture* texture)
{
	this->addDiffuseMap(texture);
}

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, Texture* texture)
{
	this->ambientColor = ambient;
	this->diffuseColor = diffuse;
	this->specularColor = specular;
	this->shininess = shininess;
	this->diffuseTexture = texture;
}

void Material::addDiffuseMap(Texture* diffuseTexture)
{
	this->useDiffuseMap = true;
	this->diffuseTexture = diffuseTexture;
}

void Material::addSpecularMap(Texture* specularTexture)
{
	this->useSpecularMap = true;
	this->specularTexture = specularTexture;
}

void Material::addNormalMap(Texture* normalTexture)
{
	this->useNormalMap = true;
	this->normalTexture = normalTexture;
}

void Material::addHeightMap(Texture* heightTexture)
{
	this->useHeightMap = true;
	this->heightTexture = heightTexture;
}
