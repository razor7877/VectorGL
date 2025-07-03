#pragma once

#include <glm/glm.hpp>

#include "shader.hpp"
#include "components/lights/lightComponent.hpp"

class SpotLightComponent : public LightComponent
{
public:
	glm::vec3 direction;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;

	explicit SpotLightComponent(Entity* parent);

	void sendToShader(Shader* shaderProgram, unsigned int index) override;
};
