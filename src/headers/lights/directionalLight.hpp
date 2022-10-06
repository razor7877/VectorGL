#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include <lights/light.hpp>

// A directional light is a type of light that lights up every object in the world from a given direction
// It represents light sources that are very far away such as the sun

class DirectionalLight : public virtual Light
{
public:
	glm::vec3 direction;

	DirectionalLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 direction);
	void Light::sendToShader(unsigned int shaderProgramID, unsigned int index) override;
	LightType Light::getLightType() override;
};

#endif