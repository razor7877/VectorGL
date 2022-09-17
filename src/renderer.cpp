#include <iostream>

#include "headers/renderer.hpp"
#include "headers/renderObject.hpp"

void Renderer::init()
{
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	for (renderObject* object : objects)
	{
		shaderMap[object->shaderProgramID].push_back(object);
	}
	/*
	bool hasTexCoords;
	bool hasNormals;
	
	// Once the shader map is built, we can generate all buffers and other necessary data in each
	// of the renderer's objects
	for (auto& shaderObjectPair : shaderMap)
	{
		hasTexCoords = false;
		hasNormals = false;

		glUseProgram(shaderObjectPair.first);
		for (renderObject* objectPtr : shaderObjectPair.second)
		{
			if (objectPtr->getType() == renderObjectType::OBJECT_MESH)
			{
				// Because we iterate over a vector of renderObject pointers, we cannot cast it directly
				// to a Mesh pointer, however, we make sure that an object is indeed a Mesh by using
				// its getType() method, and if it is, we do a void pointer cast followed by a mesh
				// pointer cast
				void* meshPtr = (void*)objectPtr;
				Mesh object = *(Mesh*)meshPtr;
				0
				// If the object contains a material,
				if (object.material.texture.texID != 0)
				{
					hasTexCoords = true;
				}

				// If the object contains normals
				if (object.normalBO != 0)
				{
					hasNormals = true;
				}
			}
		}

		// Always enable location 0, which contains vertices data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// If one or more objects use materials, setup & enable location 1, which contains texture
		// coordinates data
		if (hasTexCoords)
		{
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
		}

		// If one or more objects use normals, setup & enable location 2, which contains normals vectors
		if (hasNormals)
		{
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(2);
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