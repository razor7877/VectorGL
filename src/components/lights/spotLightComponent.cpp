#include "components/lights/spotLightComponent.hpp"
#include "lights/lightManager.hpp"

SpotLightComponent::SpotLightComponent(Entity* parent) : LightComponent(parent), Component(parent)
{
	this->constant = 1.0f;
	this->linear = 0.045f;
	this->quadratic = 0.0075f;

	this->direction = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cutOff = glm::cos(glm::radians(12.5f));
	this->outerCutOff = glm::cos(glm::radians(15.0f));

	this->index = LightManager::getInstance().addSpotLight();
}

void SpotLightComponent::sendToShader(unsigned int shaderProgramID, unsigned int index)
{
	glUseProgram(shaderProgramID);

	std::string lightLocation = "spotLights[" + std::to_string(index) + "]";

	std::string ambientLoc = lightLocation + ".ambientColor";
	std::string diffuseLoc = lightLocation + ".diffuseColor";
	std::string specularLoc = lightLocation + ".specularColor";
	std::string positionLoc = lightLocation + ".position";
	std::string directionLoc = lightLocation + ".direction";
	std::string constantLoc = lightLocation + ".constant";
	std::string linearLoc = lightLocation + ".linear";
	std::string quadraticLoc = lightLocation + ".quadratic";
	std::string cutOffLoc = lightLocation + ".cutOff";
	std::string outerCutOffLoc = lightLocation + ".outerCutOff";

	glUniform3fv(glGetUniformLocation(shaderProgramID, ambientLoc.c_str()), 1, &this->ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, diffuseLoc.c_str()), 1, &this->diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, specularLoc.c_str()), 1, &this->specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, positionLoc.c_str()), 1, &this->parent->transform->getPosition()[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, directionLoc.c_str()), 1, &direction[0]);
	glUniform1f(glGetUniformLocation(shaderProgramID, constantLoc.c_str()), this->constant);
	glUniform1f(glGetUniformLocation(shaderProgramID, linearLoc.c_str()), this->linear);
	glUniform1f(glGetUniformLocation(shaderProgramID, quadraticLoc.c_str()), this->quadratic);
	glUniform1f(glGetUniformLocation(shaderProgramID, cutOffLoc.c_str()), this->cutOff);
	glUniform1f(glGetUniformLocation(shaderProgramID, outerCutOffLoc.c_str()), this->outerCutOff);
}