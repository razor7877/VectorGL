#include "lights/light.hpp"

Light::Light(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor)
{
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
}