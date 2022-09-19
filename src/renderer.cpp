#include <iostream>

#include "headers/renderer.hpp"
#include "headers/renderObject.hpp"

void Renderer::init()
{
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	for (renderObject* object : objects)
	{
		object->setupObject();
		shaderMap[object->shaderProgramID].push_back(object);
	}

	/*
	for (auto& shaderObjectPair : shaderMap)
	{
		glUseProgram(shaderObjectPair.first);
		for (renderObject* objectPtr : shaderObjectPair.second)
		{
			objectPtr->setupObject();
		}
	}*/
}

void Renderer::render()
{
	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& shaderObjectPair : shaderMap)
	{
		// Use the current shader, then draw all objects associated with it
		glUseProgram(shaderObjectPair.first);
		for (renderObject* objectPtr : shaderObjectPair.second)
		{
			objectPtr->drawObject();
		}
	}
}

void Renderer::end()
{

}