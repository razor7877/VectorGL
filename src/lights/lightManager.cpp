#include <iostream>

#include "lights/lightManager.hpp"

LightManager LightManager::instance;

LightManager LightManager::getInstance()
{
	return LightManager::instance;
}

LightManager::LightManager()
{
	this->shaderProgramID = {};
	this->nrDirLights = {};
	this->nrPointLights = {};
	this->nrSpotLights = {};
}

LightManager::LightManager(GLuint shaderProgramID)
{
	this->shaderProgramID = shaderProgramID;
	this->nrDirLights = {};
	this->nrPointLights = {};
	this->nrSpotLights = {};
}

void LightManager::init()
{
	// Make sure the count of each light is set to 0 in case the LightManager gets initialized again
	this->nrDirLights = {};
	this->nrPointLights = {};
	this->nrSpotLights = {};

	this->sendToShader();
}

void LightManager::sendToShader()
{
	glUniform1i(glGetUniformLocation(shaderProgramID, "nrDirLights"), nrDirLights);
	glUniform1i(glGetUniformLocation(shaderProgramID, "nrPointLights"), nrPointLights);
	glUniform1i(glGetUniformLocation(shaderProgramID, "nrSpotLights"), nrSpotLights);
}

unsigned int LightManager::addDirLight()
{
	return nrDirLights++;
}

unsigned int LightManager::addPointLight()
{
	return nrPointLights++;
}

unsigned int LightManager::addSpotLight()
{
	return nrSpotLights++;
}
