#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glad/glad.h>

#include "headers/texture.hpp"
#include "headers/cubemap.hpp"
#include "material.hpp"

// A helper class to easily produce and manage objects in the world
class Mesh
{
public:
	GLuint shaderProgramID; // The ID of the shader used to draw the object
	GLuint VAO; // The object's VAO

	GLuint VBO; // The object's VBO
	GLuint texCoordBO;
	GLuint normalBO;
	GLuint indicesBO;

	Material material;
	
	unsigned int vertSize; // The size of the object's vertices array

	glm::mat4 modelMatrix; // The object's model matrix (position in world)

	Mesh();
	// Instantiates a game object, generates the VBO, VAO and attrib pointers
	Mesh(float vertices[], unsigned int vertSize, unsigned int shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	// Used in render loop to draw vertices arrays to screen
	void drawObject();
	// Generates buffers and enables correct draw calls to use given texture
	void addMaterial(Material mat, float texCoords[], unsigned int texSize);
	void addNormals(float normals[], unsigned int normalSize);

	// Draws a skybox using the object itself and a given cubemap
	// Expects the object to be a 36 vertices cube
	void drawSkybox(Cubemap cubemap);

	// Rotates the object's model matrix using a vec3 or xyz floats
	void rotateModel(float degrees, glm::vec3 rotationPoint);
	void rotateModel(float degrees, float x, float y, float z);

	// Translate the object's model matrix using a vec3 or xyz floats
	void translateModel(glm::vec3 translation);
	void translateModel(float x, float y, float z);

	// Scales the object's model matrix using a vec3 or xyz floats
	void scaleModel(glm::vec3 scaleVec);
	void scaleModel(float scaleX, float scaleY, float scaleZ);
};

#endif