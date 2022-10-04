#include "lights/lightManager.hpp"

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

void LightManager::addLight(Light* lightPtr)
{
	lights.push_back(lightPtr);
}

void LightManager::init()
{
	for (Light* light : lights)
	{
		switch (light->getLightType())
		{
			case LightType::LIGHT_DIRLIGHT:
				light->sendToShader(shaderProgramID, nrDirLights++);
				break;

			case LightType::LIGHT_POINTLIGHT:
				light->sendToShader(shaderProgramID, nrPointLights++);
				break;

			case LightType::LIGHT_SPOTLIGHT:
				light->sendToShader(shaderProgramID, nrSpotLights++);
				break;
		}
	}

	glUniform1i(glGetUniformLocation(shaderProgramID, "nrDirLights"), nrDirLights);
	glUniform1i(glGetUniformLocation(shaderProgramID, "nrPointLights"), nrPointLights);
	glUniform1i(glGetUniformLocation(shaderProgramID, "nrSpotLights"), nrSpotLights);
}