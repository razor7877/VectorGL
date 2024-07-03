#include "components/lights/directionalLightComponent.hpp"
#include "lights/lightManager.hpp"

DirectionalLightComponent::DirectionalLightComponent(Entity* parent) : LightComponent(parent), Component(parent)
{
	this->direction = glm::vec3(0.0f, 0.0f, -1.0f);

	this->index = LightManager::getInstance().addDirLight();
}

void DirectionalLightComponent::sendToShader(unsigned int shaderProgramID, unsigned int index)
{
	std::string lightLocation = "dirLights[" + std::to_string(index) + "]";

	std::string ambientLoc = lightLocation + ".ambientColor";
	std::string diffuseLoc = lightLocation + ".diffuseColor";
	std::string specularLoc = lightLocation + ".specularColor";
	std::string directionLoc = lightLocation = ".direction";

	glUniform3fv(glGetUniformLocation(shaderProgramID, ambientLoc.c_str()), 1, &this->ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, diffuseLoc.c_str()), 1, &this->diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, specularLoc.c_str()), 1, &this->specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, directionLoc.c_str()), 1, &this->direction[0]);
}
