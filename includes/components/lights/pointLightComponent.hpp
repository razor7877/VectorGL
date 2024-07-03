#pragma once

#include "entity.hpp"
#include "components/lights/lightComponent.hpp"

class PointLightComponent : public LightComponent
{
public:
	float constant;
	float linear;
	float quadratic;

	PointLightComponent(Entity* parent);

	void LightComponent::sendToShader(unsigned int shaderProgramID, unsigned int index) override;
};