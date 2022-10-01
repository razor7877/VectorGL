#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include <glm/glm.hpp>

#include "light.hpp"

class PointLight : Light
{
public:
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic);
};

#endif