#include <iostream>

#include "renderer.hpp"
#include "renderObject.hpp"

Renderer::Renderer()
{
	this->lightManager = {};
}

Renderer::Renderer(GLuint lightShaderProgramID)
{
	lightManager = LightManager(lightShaderProgramID);
}

Renderer& Renderer::addObject(RenderObject* objectPtr)
{
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	shaderMap[objectPtr->shaderProgramID].push_back(objectPtr);

	objectPtr->setupObject();
	return *this;
}

Renderer& Renderer::removeObject(RenderObject* objectPtr)
{
	std::vector<RenderObject*>* vector = &this->shaderMap[objectPtr->shaderProgramID];
	std::cout << vector->size() << std::endl;
	vector->erase(std::remove(vector->begin(), vector->end(), objectPtr), vector->end());
	std::cout << vector->size() << std::endl;
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
}

void Renderer::render()
{
	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& [shader, object] : shaderMap)
	{
		// Use the current shader, then draw all objects associated with it
		glUseProgram(shader);
		for (RenderObject* objectPtr : object)
		{
			objectPtr->drawObject();
		}
	}
}

void Renderer::end()
{

}