#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include <vector>

#include <glad/glad.h>

#include "light.hpp"
#include "directionalLight.hpp"
#include "pointLight.hpp"
#include "spotLight.hpp"

// A lightmanager serves to manages multiple lights. It is used in a renderer, and serves to easily add
// different lights to a scene and seamlessly handles sending all needed data to the shaders
class LightManager
{
public:
	static LightManager getInstance();

	GLuint shaderProgramID;

	unsigned int nrDirLights;
	unsigned int nrPointLights;
	unsigned int nrSpotLights;

	LightManager();
	LightManager(GLuint shaderProgramID);

	unsigned int addDirLight();
	unsigned int addPointLight();
	unsigned int addSpotLight();

	void init();
	// Sends the current number of each type of light to the shader
	void sendToShader();

private:
	static LightManager instance;
};

#endif