#pragma once

#include "Shader.hpp"
#include "components/lights/LightComponent.hpp"

class PointLightComponent : public LightComponent
{
public:
	float constant;
	float linear;
	float quadratic;

	explicit PointLightComponent(Entity* parent);

	void sendToShader(Shader* shaderProgram, unsigned int index) override;
};
