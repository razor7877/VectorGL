#include "components/lights/lightComponent.hpp"
#include "entity.hpp"

LightComponent::LightComponent(Entity* parent) : Component(parent)
{
	this->shaderProgramID = 0;
	this->index = 0;

	this->isEnabled = false;

	this->ambientColor = glm::vec3(1.0f);
	this->diffuseColor = glm::vec3(1.0f);
	this->specularColor = glm::vec3(1.0f);
}

void LightComponent::setShader(GLuint shaderProgramID, int index)
{
	this->shaderProgramID = shaderProgramID;
	this->index = index;

	this->isEnabled = true;
}