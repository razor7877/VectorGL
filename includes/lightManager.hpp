#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include "shader.hpp"

// A LightManager serves to manages multiple lights. It is used in a renderer, and serves to easily add
// different lights to a scene and seamlessly handles sending all needed data to the shaders
class LightManager
{
public:
	static LightManager& getInstance();

	LightManager(LightManager const&) = delete;
	LightManager& operator=(LightManager const&) = delete;

	Shader* shaderProgram;

	unsigned int addDirLight();
	unsigned int addPointLight();
	unsigned int addSpotLight();

	void init();
	// Sends the current number of each type of light to the shader
	void sendToShader() const;

private:
	static LightManager instance;

	LightManager();

	unsigned int nrDirLights;
	unsigned int nrPointLights;
	unsigned int nrSpotLights;
};

#endif