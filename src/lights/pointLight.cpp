#include <string>

#include <glad/glad.h>

#include "lights/pointLight.hpp"

PointLight::PointLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic)
	: Light(ambientColor, diffuseColor, specularColor)
{
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

void PointLight::sendToShader(unsigned int shaderProgramID, unsigned int index)
{
	std::string lightLocation = "pointLights[" + std::to_string(index) + ']';

	std::string ambientLoc = lightLocation + ".ambientColor";
	std::string diffuseLoc = lightLocation + ".diffuseColor";
	std::string specularLoc = lightLocation + ".specularColor";
	std::string positionLoc = lightLocation + ".position";
	std::string constantLoc = lightLocation + ".constant";
	std::string linearLoc = lightLocation + ".linear";
	std::string quadraticLoc = lightLocation + ".quadratic";

	glUniform3fv(glGetUniformLocation(shaderProgramID, ambientLoc.c_str()), 1, &ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, diffuseLoc.c_str()), 1, &diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, specularLoc.c_str()), 1, &specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, positionLoc.c_str()), 1, &position[0]);
	glUniform1f(glGetUniformLocation(shaderProgramID, constantLoc.c_str()), constant);
	glUniform1f(glGetUniformLocation(shaderProgramID, linearLoc.c_str()), linear);
	glUniform1f(glGetUniformLocation(shaderProgramID, quadraticLoc.c_str()), quadratic);
}

LightType PointLight::getLightType()
{
	return LightType::LIGHT_POINTLIGHT;
}