#include <iostream>

#include "renderer.hpp"
#include "renderObject.hpp"

void Renderer::addObject(RenderObject* objectPtr)
{
	objects.push_back(objectPtr);
}

void Renderer::init()
{
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