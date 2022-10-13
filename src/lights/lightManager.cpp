#include <iostream>

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
	// Make sure the count of each light is set to 0 in case the LightManager gets initialized again
	this->nrDirLights = {};
	this->nrPointLights = {};
	this->nrSpotLights = {};

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

std::vector<DirectionalLight*> LightManager::getDirLights()
{
	std::vector<DirectionalLight*> dirLights;

	for (Light* light : lights)
	{
		if (light->getLightType() == LightType::LIGHT_DIRLIGHT)
		{
			DirectionalLight* lightPtr = dynamic_cast<DirectionalLight*>(light);
			if (lightPtr == NULL)
			{
				std::cout << "LightManager::getDirLights() - Got nullptr when attempting to cast Light object to DirectionalLight" << std::endl;
			}
			else
			{
				dirLights.push_back(lightPtr);
			}
		}
	}

	return dirLights;
}

std::vector<PointLight*> LightManager::getPointLights()
{
	std::vector<PointLight*> pointLights;

	for (Light* light : lights)
	{
		if (light->getLightType() == LightType::LIGHT_POINTLIGHT)
		{
			PointLight* lightPtr = dynamic_cast<PointLight*>(light);
			if (lightPtr == NULL)
			{
				std::cout << "LightManager::getPointLights() - Got nullptr when attempting to cast Light object to PointLight" << std::endl;
			}
			else
			{
				pointLights.push_back(lightPtr);
			}
		}
	}

	return pointLights;
}

std::vector<SpotLight*> LightManager::getSpotLights()
{
	std::vector<SpotLight*> spotLights;

	for (Light* light : lights)
	{
		if (light->getLightType() == LightType::LIGHT_SPOTLIGHT)
		{
			SpotLight* lightPtr = dynamic_cast<SpotLight*>(light);
			if (lightPtr == NULL)
			{
				std::cout << "LightManager::getSpotLights() - Got nullptr when attempting to cast Light object to SpotLight" << std::endl;
			}
			else
			{
				spotLights.push_back(lightPtr);
			}
		}
	}

	return spotLights;
}