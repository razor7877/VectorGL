#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "headers/gameObject.hpp"

gameObject::gameObject(float vertices[], unsigned int vertSize, unsigned int shaderProgramID, glm::vec3 position)
{
	this->shaderProgramID = shaderProgramID;
	this->vertSize = vertSize;

	this->modelMatrix = glm::translate(glm::mat4(1.0f), position);

	this->textureBuffer = {};
	this->hasTexture = false;
	this->texture = Texture();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void gameObject::drawObject()
{
	glUseProgram(shaderProgramID);

	// Make sure the object's VAO is bound
	glBindVertexArray(VAO);

	// If a texture is associated with the object, activate texture unit 0 and bind texture
	if (hasTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		this->texture.bindTexture();
	}

	// Send the object's model matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

	// Draw the object's vertices as triangles
	glDrawArrays(GL_TRIANGLES, 0, vertSize);
}

void gameObject::addTexture(Texture tex, float texCoords[], unsigned int texSize)
{
	// Makes sure the object's VAO is bound
	glBindVertexArray(VAO);

	texture = tex;
	hasTexture = true;

	// Generates a buffer to store texture coordinates data
	glGenBuffers(1, &textureBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, texSize, texCoords, GL_STATIC_DRAW);

	// Passed as vec2 at location 1 in shader
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

void gameObject::drawSkybox(Cubemap cubemap)
{
	glUseProgram(shaderProgramID);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.texID);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void gameObject::rotateModel(float degrees, glm::vec3 rotationPoint)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), rotationPoint);
}

void gameObject::rotateModel(float degrees, float x, float y, float z)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), glm::vec3(x, y, z));
}

void gameObject::translateModel(glm::vec3 translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
}

void gameObject::translateModel(float x, float y, float z)
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
}

void gameObject::scaleModel(glm::vec3 scaleVec)
{
	modelMatrix = glm::scale(modelMatrix, scaleVec);
}

void gameObject::scaleModel(float scaleX, float scaleY, float scaleZ)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, scaleZ));
}