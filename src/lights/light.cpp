#include "lights/light.hpp"

Light::Light()
{
	this->ambientColor = glm::vec3(1.0f);
	this->diffuseColor = glm::vec3(1.0f);
	this->specularColor = glm::vec3(1.0f);
}

Light::Light(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor)
{
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
}