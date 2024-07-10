#include "material.hpp"

Material::Material()
{

}

Material::Material(Texture* texture)
{
	this->ambient = glm::vec3(1.0f);
	this->diffuse = glm::vec3(0.1f);
	this->specular = glm::vec3(0.05f);
	this->shininess = 128.0f;
	this->texture = texture;
}

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, Texture* texture)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->texture = texture;
}