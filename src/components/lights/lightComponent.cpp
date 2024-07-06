#include "components/lights/lightComponent.hpp"
#include "entity.hpp"
#include "lights/lightManager.hpp"

LightComponent::LightComponent(Entity* parent) : Component(parent)
{
	this->ambientColor = glm::vec3(1.0f);
	this->diffuseColor = glm::vec3(1.0f);
	this->specularColor = glm::vec3(1.0f);

	this->index = 0;
	this->shaderProgram = LightManager::getInstance().shaderProgram;
}

void LightComponent::start()
{

}

void LightComponent::update()
{
	this->sendToShader(this->shaderProgram, this->index);
}