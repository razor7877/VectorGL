#include <iostream>

#include <glad/glad.h>

#include "skybox.hpp"

// Setup default mesh variables with parent constructor, then set class specific variables
Skybox::Skybox(float vertices[], unsigned int vertSize, GLuint shaderProgramID, Cubemap cubemap, glm::vec3 position)
	: Mesh(vertices, vertSize, shaderProgramID, position)
{
	this->cubemap = cubemap;
}

// Draw the skybox
void Skybox::drawObject()
{
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.texID);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void Skybox::setupObject()
{
	Mesh::setupObject();
	cubemap.setupObject();
}