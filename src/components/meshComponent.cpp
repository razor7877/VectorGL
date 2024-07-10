#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "logger.hpp"
#include "components/meshComponent.hpp"
#include "entity.hpp"

MeshComponent::MeshComponent(Entity* parent) : Component(parent)
{

}

MeshComponent::~MeshComponent()
{
	Logger::logDebug("Calling MeshComponent destructor");

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
		this->hasIndices = true;

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

		// We sort the textures from the vector into their own members in the mesh for easier use later
		for (int i = 0; i < textures.size(); i++)
		{
			switch (this->textures[i]->type)
			{
				case TextureType::TEXTURE_DIFFUSE:
					this->material.useDiffuseMap = true;
					this->material.diffuseTexture = this->textures[i];
					break;

				case TextureType::TEXTURE_SPECULAR:
					this->material.useSpecularMap = true;
					this->material.specularTexture = this->textures[i];
					break;

				case TextureType::TEXTURE_NORMAL:
					this->material.useNormalMap = true;
					this->material.normalTexture = this->textures[i];
					break;

				case TextureType::TEXTURE_HEIGHT:
					this->material.useHeightMap = true;
					this->material.heightTexture = this->textures[i];
					break;
			}
		}
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

	this->verticesCount = this->vertices.size();
	this->indicesCount = this->indices.size();

	// No need to store the entire buffers in memory once they're on the GPU
	this->vertices.clear();
	this->texCoords.clear();
	this->normals.clear();
	this->indices.clear();
}

void MeshComponent::update()
{
	this->shaderProgram->use();

	// Make sure the object's VAO is bound
	glBindVertexArray(VAO);

	// Send all data relevant to textures
	this->shaderProgram->setInt("use_vertex_colors", this->useVertexColors);
	this->shaderProgram->setInt("use_diffuse_map", this->material.useDiffuseMap);
	this->shaderProgram->setInt("use_specular_map", this->material.useSpecularMap);
	this->shaderProgram->setInt("use_normal_map", this->material.useNormalMap);
	this->shaderProgram->setInt("use_height_map", this->material.useHeightMap);

	// Bind all the textures needed
	if (this->material.useDiffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		this->shaderProgram->setInt("texture_diffuse", 0);
		this->material.diffuseTexture->bindTexture();
	}
	else
		this->shaderProgram->setVec3("diffuse_color", this->material.diffuseColor);

	if (this->material.useSpecularMap)
	{
		glActiveTexture(GL_TEXTURE1);
		this->shaderProgram->setInt("texture_specular", 1);
		this->material.specularTexture->bindTexture();
	}

	if (this->material.useNormalMap)
	{
		glActiveTexture(GL_TEXTURE2);
		this->shaderProgram->setInt("texture_normal", 2);
		this->material.normalTexture->bindTexture();
	}

	if (this->material.useHeightMap)
	{
		glActiveTexture(GL_TEXTURE3);
		this->shaderProgram->setInt("texture_height", 3);
		this->material.heightTexture->bindTexture();
	}

	// Send the model matrix & material data
	this->shaderProgram
		->setMat4("model", this->parent->transform->getModelMatrix())
		->setVec3("material.ambient", this->material.ambientColor)
		->setVec3("material.diffuse", this->material.diffuseColor)
		->setVec3("material.specular", this->material.specularColor)
		->setFloat("material.shininess", this->material.shininess);

	// Indexed drawing
	if (this->hasIndices)
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indicesCount, GL_UNSIGNED_INT, 0);
	else // Normal drawing
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticesCount * sizeof(float));

	for (int i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE0);
}

void MeshComponent::setupMesh(float vertices[], unsigned int vertSize, Shader* shaderProgram, glm::vec3 position)
{
	this->vertices.insert(this->vertices.end(), &vertices[0], &vertices[vertSize / sizeof(float)]);
	this->shaderProgram = shaderProgram;
	this->parent->transform->setModelMatrix(position, glm::vec3(0), glm::vec3(1.0f));

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};
}

void MeshComponent::setupMesh(
	std::vector<float> vertices,
	std::vector<float> texCoords,
	std::vector<float> normals,
	std::vector<unsigned int> indices,
	std::vector<Texture*> textures, Shader* shaderProgram,
	glm::vec3 position)
{
	this->vertices = vertices;
	this->shaderProgram = shaderProgram;
	this->parent->transform->setModelMatrix(position, glm::vec3(0), glm::vec3(1.0f));
	this->texCoords = texCoords;
	this->textures = textures;

	addNormals(&normals[0], (GLsizei)normals.size() * sizeof(float));
	addIndices(&indices[0], (GLsizei)indices.size() * sizeof(unsigned int));

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};
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

void MeshComponent::setDiffuseColor(glm::vec3 color)
{
	this->material.diffuseColor = color;
}