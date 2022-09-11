#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glad/glad.h>

#include "headers/texture.hpp"

// A helper class to easily produce and manage objects in the world
class gameObject
{
public:
	unsigned int shaderProgramID; // The ID of the shader used to draw the object
	unsigned int VBO; // The object's VBO
	unsigned int VAO; // The object's VAO
	unsigned int vertSize; // The size of the object's vertices array

	glm::mat4 modelMatrix; // The object's model matrix (position in world)

	unsigned int textureBuffer;
	bool hasTexture;
	Texture texture;

	// Instantiates a game object, generates the VBO, VAO and attrib pointers
	gameObject(float vertices[], unsigned int vertSize, unsigned int shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	// Used in render loop to draw vertices arrays to screen
	void drawObject();
	// Generates buffers and enables correct draw calls to use given texture
	void addTexture(Texture tex, float texCoords[], unsigned int texSize);

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