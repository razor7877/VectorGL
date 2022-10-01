#include "lights/directionalLight.hpp"

directionalLight::directionalLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 direction)
	: Light(ambientColor, diffuseColor, specularColor)
{
	this->direction = direction;
}