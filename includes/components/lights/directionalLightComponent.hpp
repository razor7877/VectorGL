#pragma once

#include "components/lights/lightComponent.hpp"

class DirectionalLightComponent : public virtual LightComponent
{
public:
	glm::vec3 direction;

	DirectionalLightComponent(Entity* parent);

	void LightComponent::sendToShader(unsigned int shaderProgramID, unsigned int index) override;
};