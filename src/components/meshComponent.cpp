#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "components/meshComponent.hpp"

MeshComponent::MeshComponent() : Component()
{
	this->shaderProgramID = {};

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};
}

MeshComponent::~MeshComponent()
{
	std::cout << "Calling MeshComponent destructor" << std::endl;

	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->indicesBO);
	glDeleteBuffers(1, &this->texCoordBO);
	glDeleteBuffers(1, &this->normalBO);

	glDeleteVertexArrays(1, &this->VAO);
}

void MeshComponent::start()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// If the MeshComponent uses indices
	if (indices.size() > 0)
	{
		glGenBuffers(1, &indicesBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// If the MeshComponent uses textures
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

void MeshComponent::update()
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
			std::string name = textures[i]->type;

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
			glBindTexture(GL_TEXTURE_2D, textures[i]->texID);
		}
	}

	// Send the object's model matrix to the shader
	//glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, &this->transform->getModelMatrix()[0][0]);

	if (indices.size() == 0)
	{
		// Draw the object's vertices as triangles
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size() * sizeof(float));
	}
	else
	{
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	}
}

// Add texture coordinates data to the MeshComponent
MeshComponent& MeshComponent::addTexCoords(std::vector<float> texCoords)
{
	this->texCoords = texCoords;
	return *this;
}

MeshComponent& MeshComponent::addTexCoords(float texCoords[], unsigned int texSize)
{
	this->texCoords.insert(this->texCoords.end(), &texCoords[0], &texCoords[texSize / sizeof(float)]);
	return *this;
}

MeshComponent& MeshComponent::addTexture(Texture* texture)
{
	textures.insert(textures.end(), texture);
	return *this;
}

// Add normals data to the MeshComponent
MeshComponent& MeshComponent::addNormals(float normals[], unsigned int normalSize)
{
	this->normals.insert(this->normals.end(), &normals[0], &normals[normalSize / sizeof(float)]);
	return *this;
}

// Add indices data to the MeshComponent
MeshComponent& MeshComponent::addIndices(unsigned int indices[], unsigned int indicesSize)
{
	this->indices.insert(this->indices.end(), &indices[0], &indices[indicesSize / sizeof(unsigned int)]);
	return *this;
}