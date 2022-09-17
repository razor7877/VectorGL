#include <iostream>

#include "headers/renderer.hpp"

void Renderer::init()
{
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	for (renderObject* object : objects)
	{
		shaderMap[object->shaderProgramID].push_back(object);
	}
}

void Renderer::render()
{
	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& shaderMeshPair : shaderMap)
	{
		glUseProgram(shaderMeshPair.first);
		for (renderObject* objectPtr : shaderMeshPair.second)
		{
			objectPtr->drawObject();
		}
	}
}

void Renderer::end()
{

}