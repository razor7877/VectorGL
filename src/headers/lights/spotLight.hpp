#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <glm/glm.hpp>

#include "pointLight.hpp"

class SpotLight : PointLight
{
public:
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 direction, float cutOff, float outerCutOff);
};

#endif