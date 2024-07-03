#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "entity.hpp"
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

	void setShader(GLuint shaderProgramID, int index);
	void virtual sendToShader(unsigned int shaderProgramID, unsigned int index) = 0;

protected:
	GLuint shaderProgramID;
	int index;

	bool isEnabled;
};