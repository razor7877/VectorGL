#include <iostream>

#include "renderer.hpp"
#include "renderObject.hpp"

Renderer::Renderer(GLuint lightShaderProgramID)
{
	lightManager = LightManager(lightShaderProgramID);
}

Renderer& Renderer::addObject(RenderObject* objectPtr)
{
	objects.push_back(objectPtr);
	return *this;
}

Renderer& Renderer::addLight(Light* lightPtr)
{
	lightManager.addLight(lightPtr);
	return *this;
}

void Renderer::init()
{
	// Initializes the light manager if it was setup to send all required data to the shader
	if (lightManager.shaderProgramID != 0)
	{
		lightManager.init();
	}

	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	for (RenderObject* object : objects)
	{
		object->setupObject();
		shaderMap[object->shaderProgramID].push_back(object);
	}
}

void Renderer::render()
{
	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& shaderObjectPair : shaderMap)
	{
		// Use the current shader, then draw all objects associated with it
		glUseProgram(shaderObjectPair.first);
		for (RenderObject* objectPtr : shaderObjectPair.second)
		{
			objectPtr->drawObject();
		}
	}
}

void Renderer::end()
{

}