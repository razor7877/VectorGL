#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "headers/Mesh.hpp"

Mesh::Mesh()
{

}

Mesh::Mesh(float vertices[], unsigned int vertSize, GLuint shaderProgramID, glm::vec3 position)
{
	this->vertices.insert(this->vertices.end(), &vertices[0], &vertices[vertSize / sizeof(float)]);
	this->shaderProgramID = shaderProgramID;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position);

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};
}

Mesh::Mesh(std::vector<float> vertices, std::vector<float> texCoords, std::vector<float> normals, std::vector<unsigned int> indices, std::vector<Texture> textures, GLuint shaderProgramID, glm::vec3 position)
{
	this->vertices = vertices;
	this->shaderProgramID = shaderProgramID;
	this->modelMatrix = glm::translate(glm::mat4(1.0f), position);
	this->texCoords = texCoords;
	this->textures = textures;

	addNormals(&normals[0], normals.size() * sizeof(float));
	addIndices(&indices[0], indices.size() * sizeof(unsigned int));

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};
}

void Mesh::drawObject()
{
	// Make sure the object's VAO is bound
	glBindVertexArray(VAO);

	if (textures.size() != 0)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++);
			}
			else if (name == "texture_normal")
			{
				number = std::to_string(normalNr++);
			}
			else if (name == "texture_height")
			{
				number = std::to_string(heightNr++);
			}

			glUniform1i(glGetUniformLocation(shaderProgramID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].texID);
		}
	}

	// Send the object's model matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

	if (indices.size() == 0)
	{
		// Draw the object's vertices as triangles
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * sizeof(float));
	}
	else
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Mesh::setupObject()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// If the mesh uses indices
	if (indices.size() > 0)
	{
		glGenBuffers(1, &indicesBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// If the mesh uses textures
	if (textures.size() > 0)
	{
		// Generates a buffer to store texture coordinates data
		glGenBuffers(1, &texCoordBO);

		glBindBuffer(GL_ARRAY_BUFFER, texCoordBO);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
	}

	// If the object uses normals
	if (normals.size() > 0)
	{
		glGenBuffers(1, &normalBO);

		glBindBuffer(GL_ARRAY_BUFFER, normalBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}
}

renderObjectType Mesh::getType()
{
	return renderObjectType::OBJECT_MESH;
}

// Add texture coordinates data to the mesh
void Mesh::addTexCoords(std::vector<float> texCoords)
{
	this->texCoords = texCoords;
}

void Mesh::addTexCoords(float texCoords[], unsigned int texSize)
{
	this->texCoords.insert(this->texCoords.end(), &texCoords[0], &texCoords[texSize / sizeof(float)]);
}

void Mesh::addTexture(Texture texture)
{
	textures.insert(textures.end(), texture);
}

// Add normals data to the mesh
void Mesh::addNormals(float normals[], unsigned int normalSize)
{
	this->normals.insert(this->normals.end(), &normals[0], &normals[normalSize / sizeof(float)]);
}

// Add indices data to the mesh
void Mesh::addIndices(unsigned int indices[], unsigned int indicesSize)
{
	this->indices.insert(this->indices.end(), &indices[0], &indices[indicesSize / sizeof(unsigned int)]);
}

void Mesh::rotateMesh(float degrees, glm::vec3 rotationPoint)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), rotationPoint);
}

void Mesh::rotateMesh(float degrees, float x, float y, float z)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), glm::vec3(x, y, z));
}

void Mesh::translateMesh(glm::vec3 translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
}

void Mesh::translateMesh(float x, float y, float z)
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
}

void Mesh::scaleMesh(glm::vec3 scaleVec)
{
	modelMatrix = glm::scale(modelMatrix, scaleVec);
}

void Mesh::scaleMesh(float scaleX, float scaleY, float scaleZ)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, scaleZ));
}