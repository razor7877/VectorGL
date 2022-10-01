#include "lights/pointLight.hpp"

PointLight::PointLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic)
	: Light(ambientColor, diffuseColor, specularColor)
{
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}