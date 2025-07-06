#pragma once

#include "components/lights/LightComponent.hpp"
#include "Shader.hpp"

class DirectionalLightComponent : public virtual LightComponent
{
public:
	glm::vec3 direction{};

	explicit DirectionalLightComponent(Entity* parent);

	void sendToShader(Shader* shaderProgram, unsigned int index) override;
};
