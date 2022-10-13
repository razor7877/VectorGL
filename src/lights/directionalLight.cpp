#include <string>

#include <glad/glad.h>

#include "lights/directionalLight.hpp"

DirectionalLight::DirectionalLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 direction)
	: Light(ambientColor, diffuseColor, specularColor)
{
	this->direction = direction;
}

void DirectionalLight::sendToShader(unsigned int shaderProgramID, unsigned int index)
{
	std::string lightLocation = "dirLights[" + std::to_string(index) + "]";

	std::string ambientLoc = lightLocation + ".ambientColor";
	std::string diffuseLoc = lightLocation + ".diffuseColor";
	std::string specularLoc = lightLocation + ".specularColor";
	std::string directionLoc = lightLocation = ".direction";

	glUniform3fv(glGetUniformLocation(shaderProgramID, ambientLoc.c_str()), 1, &ambientColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, diffuseLoc.c_str()), 1, &diffuseColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, specularLoc.c_str()), 1, &specularColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, directionLoc.c_str()), 1, &direction[0]);
}

LightType DirectionalLight::getLightType()
{
	return LightType::LIGHT_DIRLIGHT;
}