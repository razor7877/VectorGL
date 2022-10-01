#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include <lights/light.hpp>

class directionalLight : Light
{
public:
	glm::vec3 direction;

	directionalLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 direction);
};

#endif