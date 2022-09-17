#include <iostream>

#include "headers/renderer.hpp"

void Renderer::init()
{
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	for (Mesh* mesh : meshes)
	{
		shaderMap[mesh->shaderProgramID].push_back(mesh);
	}
}

void Renderer::render()
{
	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& shaderMeshPair : shaderMap)
	{
		glUseProgram(shaderMeshPair.first);
		for (Mesh* meshPtr : shaderMeshPair.second)
		{
			meshPtr->drawObject();
		}
	}
}

void Renderer::end()
{

}