#include <string>

#include <utilities/glad.h>

#include "components/lights/pointLightComponent.hpp"
#include "lightManager.hpp"

PointLightComponent::PointLightComponent(Entity* parent) : LightComponent(parent), Component(parent)
{
	this->constant = 1.0f;
	this->linear = 0.045f;
	this->quadratic = 0.0075f;

	this->index = LightManager::getInstance().addPointLight();
}

void PointLightComponent::sendToShader(Shader* shaderProgram, unsigned int index)
{
	glUseProgram(shaderProgram->getID());

	std::string lightLocation = "pointLights[" + std::to_string(index) + "]";
	std::string ambientLoc = lightLocation + ".ambientColor";
	std::string diffuseLoc = lightLocation + ".diffuseColor";
	std::string specularLoc = lightLocation + ".specularColor";
	std::string positionLoc = lightLocation + ".position";
	std::string constantLoc = lightLocation + ".constant";
	std::string linearLoc = lightLocation + ".linear";
	std::string quadraticLoc = lightLocation + ".quadratic";

	glUniform3fv(glGetUniformLocation(shaderProgram->getID(), ambientLoc.c_str()), 1, &this->ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->getID(), diffuseLoc.c_str()), 1, &this->diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->getID(), specularLoc.c_str()), 1, &this->specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram->getID(), positionLoc.c_str()), 1, &this->parent->getTransform()->getPosition()[0]);
	glUniform1f(glGetUniformLocation(shaderProgram->getID(), constantLoc.c_str()), this->constant);
	glUniform1f(glGetUniformLocation(shaderProgram->getID(), linearLoc.c_str()), this->linear);
	glUniform1f(glGetUniformLocation(shaderProgram->getID(), quadraticLoc.c_str()), this->quadratic);
}