#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <glm/glm.hpp>

#include "pointLight.hpp"

// A spot light is a type of light placed at a xyz point in space that emits a cone of light in a specific direction
// It represents lights like a flashlight or a car's headlights

class SpotLight : public virtual Light
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 direction, float cutOff, float outerCutOff);
	void Light::sendToShader(unsigned int shaderProgramID, unsigned int index) override;
	LightType Light::getLightType() override;
};

#endif