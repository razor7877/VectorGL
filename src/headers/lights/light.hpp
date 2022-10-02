#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

enum class LightType
{
	LIGHT_DIRLIGHT,
	LIGHT_POINTLIGHT,
	LIGHT_SPOTLIGHT
};

// The light class is an interface implemented by other classes (directional lights, point light, spotlights)
// Each light object parameters can then be sent to shaders to calculate lighting
class Light
{
public:
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	Light();
	Light(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
	void virtual sendToShader(unsigned int shaderProgramID, unsigned int index) = 0;
	LightType virtual getLightType() = 0;
};

#endif