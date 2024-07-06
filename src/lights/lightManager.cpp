#include <iostream>

#include "lights/lightManager.hpp"

LightManager LightManager::instance;

LightManager& LightManager::getInstance()
{
	return LightManager::instance;
}

LightManager::LightManager()
{
	this->shaderProgram = {};
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
	glUseProgram(this->shaderProgram->ID);

	glUniform1i(glGetUniformLocation(this->shaderProgram->ID, "nrDirLights"), this->nrDirLights);
	glUniform1i(glGetUniformLocation(this->shaderProgram->ID, "nrPointLights"), this->nrPointLights);
	glUniform1i(glGetUniformLocation(this->shaderProgram->ID, "nrSpotLights"), this->nrSpotLights);
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
