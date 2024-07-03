#include <iostream>

#include "lights/lightManager.hpp"

LightManager LightManager::instance;

LightManager& LightManager::getInstance()
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

void LightManager::init()
{
	this->sendToShader();
}

void LightManager::sendToShader()
{
	glUseProgram(this->shaderProgramID);

	glUniform1i(glGetUniformLocation(this->shaderProgramID, "nrDirLights"), this->nrDirLights);
	glUniform1i(glGetUniformLocation(this->shaderProgramID, "nrPointLights"), this->nrPointLights);
	glUniform1i(glGetUniformLocation(this->shaderProgramID, "nrSpotLights"), this->nrSpotLights);
}

unsigned int LightManager::addDirLight()
{
	this->nrDirLights++;
	this->sendToShader();

	return this->nrDirLights - 1;
}

unsigned int LightManager::addPointLight()
{
	this->nrPointLights++;
	this->sendToShader();

	return this->nrPointLights - 1;
}

unsigned int LightManager::addSpotLight()
{
	this->nrSpotLights++;
	this->sendToShader();

	return this->nrSpotLights - 1;
}
