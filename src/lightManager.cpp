#include "lightManager.hpp"

// TODO : Get rid of this or make it better somehow

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
	this->nrDirLights = {};
	this->nrPointLights = {};
	this->nrSpotLights = {};
	this->sendToShader();
}

void LightManager::sendToShader() const
{
	this->shaderProgram->use();

	this->shaderProgram->setInt("nrDirLights", this->nrDirLights);
	this->shaderProgram->setInt("nrPointLights", this->nrPointLights);
	this->shaderProgram->setInt("nrSpotLights", this->nrSpotLights);
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
