#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include <vector>

#include <glad/glad.h>

#include "light.hpp"

// A lightmanager serves to manages multiple lights. It is used in a renderer, and serves to easily add
// different lights to a scene and seamlessly handles sending all needed data to the shaders
class LightManager
{
public:
	GLuint shaderProgramID;

	// Stores a vector of Light objects, which can be objects from any classes that implement the Light
	// interface (DirectionalLight, PointLight, SpotLight ...)
	std::vector<Light*> lights;
	unsigned int nrDirLights;
	unsigned int nrPointLights;
	unsigned int nrSpotLights;

	LightManager();
	LightManager(GLuint shaderProgramID);

	void addLight(Light* light);

	void init();
};

#endif