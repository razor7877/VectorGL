#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include <glm/glm.hpp>

#include "light.hpp"

// A point light is a type of light placed at a xyz point in space that emits light all around itself
// It represents any sort of simple light sources

class PointLight : public virtual Light
{
public:
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic);
	void Light::sendToShader(unsigned int shaderProgramID, unsigned int index) override;
	LightType Light::getLightType() override;
};

#endif