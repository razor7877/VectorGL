#pragma once

#include <utilities/glad.h>
#include <glm/glm.hpp>

#include "entity.hpp"
#include "shader.hpp"
#include "components/component.hpp"

/// <summary>
/// The base component for creating different types of lights
/// </summary>
class LightComponent : public virtual Component
{
public:
	// The colors of each of the light's components
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	LightComponent(Entity* parent);

	void start() override;
	void update(float deltaTime) override;

	void virtual sendToShader(Shader* shaderProgram, unsigned int index) = 0;

protected:
	Shader* shaderProgram;
	int index;

	bool isEnabled;
};
