#include "lights/spotLight.hpp"

SpotLight::SpotLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 direction, float cutOff, float outerCutOff)
	: PointLight(ambientColor, diffuseColor, specularColor, position, constant, linear, quadratic)
{
	this->direction = direction;
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
}