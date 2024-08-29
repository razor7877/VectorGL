#include <glm/glm.hpp>
#include <glm/glm/ext/matrix_transform.hpp>

#include "components/lights/spotLightComponent.hpp"
#include "lightManager.hpp"

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

void SpotLightComponent::sendToShader(Shader* shaderProgram, unsigned int index)
{
	glUseProgram(shaderProgram->ID);

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
	
	glm::vec3 rotation = this->parent->transform->getRotation();

	// Calculate the new front vector
	glm::vec3 newDirection{};
	newDirection.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	newDirection.y = sin(glm::radians(rotation.y));
	newDirection.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	newDirection = glm::normalize(newDirection);

	glUniform3fv(glGetUniformLocation(shaderProgram->ID, ambientLoc.c_str()), 1, &this->ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, diffuseLoc.c_str()), 1, &this->diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, specularLoc.c_str()), 1, &this->specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, positionLoc.c_str()), 1, &this->parent->transform->getPosition()[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, directionLoc.c_str()), 1, &newDirection[0]);
	glUniform1f(glGetUniformLocation(shaderProgram->ID, constantLoc.c_str()), this->constant);
	glUniform1f(glGetUniformLocation(shaderProgram->ID, linearLoc.c_str()), this->linear);
	glUniform1f(glGetUniformLocation(shaderProgram->ID, quadraticLoc.c_str()), this->quadratic);
	glUniform1f(glGetUniformLocation(shaderProgram->ID, cutOffLoc.c_str()), this->cutOff);
	glUniform1f(glGetUniformLocation(shaderProgram->ID, outerCutOffLoc.c_str()), this->outerCutOff);
}