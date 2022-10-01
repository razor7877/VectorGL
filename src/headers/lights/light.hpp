#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

// The light class is an interface implemented by other classes (directional lights, point light, spotlights)
// Each light object parameters can then be sent to shaders to calculate lighting

class Light
{
public:
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	Light(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
};

#endif