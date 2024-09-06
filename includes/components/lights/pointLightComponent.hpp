#pragma once

#include "entity.hpp"
#include "shader.hpp"
#include "components/lights/lightComponent.hpp"

class PointLightComponent : public LightComponent
{
public:
	float constant;
	float linear;
	float quadratic;

	PointLightComponent(Entity* parent);

	void sendToShader(Shader* shaderProgram, unsigned int index) override;
};
