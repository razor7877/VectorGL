#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "logger.hpp"
#include "components/meshComponent.hpp"
#include "entity.hpp"
#include "materials/phongMaterial.hpp"
#include "materials/pbrMaterial.hpp"
#include "materials/material.hpp"
#include "utilities/geometry.hpp"

const std::string MeshComponent::MODEL = "model";
const std::string MeshComponent::NORMAL_MATRIX = "normalMatrix";

MeshComponent::MeshComponent(Entity* parent) : Component(parent)
{

}

MeshComponent::~MeshComponent()
{
	Logger::logDebug("Calling MeshComponent destructor", "meshComponent.cpp");
	 
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->indicesBO);
	glDeleteBuffers(1, &this->texCoordBO);
	glDeleteBuffers(1, &this->normalBO);

	glDeleteVertexArrays(1, &this->VAO);

	this->material.reset();
}

void MeshComponent::start()
{
	// We calculate the normals if none are provided
	if (normals.size() == 0)
	{
		if (indices.size() > 0)
			this->normals = Geometry::calculateVerticesNormals(this->vertices, this->indices);
		else
			this->normals = Geometry::calculateVerticesNormals(this->vertices);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// Send the indices
	if (indices.size() > 0)
	{
		this->hasIndices = true;

		glGenBuffers(1, &indicesBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// If the MeshComponent uses textures, send them to the material
	if (textures.size() > 0 && this->material != nullptr)
		this->material->addTextures(this->textures);

	// Send texture coordinates if present
	if (texCoords.size() > 0)
	{
		// Generates a buffer to store texture coordinates data
		glGenBuffers(1, &texCoordBO);

		glBindBuffer(GL_ARRAY_BUFFER, texCoordBO);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
	}
	else if (textures.size() > 0)
		Logger::logWarning("MeshComponent has texture but no associated texture coordinates!", "meshComponent.cpp");

	// Send normals for lighting calculations
	if (normals.size() > 0)
	{
		glGenBuffers(1, &normalBO);

		glBindBuffer(GL_ARRAY_BUFFER, normalBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Send tangents for normal mapping
	if (tangents.size() > 0)
	{
		glGenBuffers(1, &tangentsBO);

		glBindBuffer(GL_ARRAY_BUFFER, tangentsBO);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), &tangents[0], GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3);
	}

	// Send bitangents for normal mapping
	if (bitangents.size() > 0)
	{
		glGenBuffers(1, &bitangentsBO);

		glBindBuffer(GL_ARRAY_BUFFER, bitangentsBO);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(float), &bitangents[0], GL_STATIC_DRAW);

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(4);
	}

	this->verticesCount = this->vertices.size();
	this->indicesCount = this->indices.size();

	// No need to store the entire buffers in memory once they're on the GPU
	this->vertices.clear();
	this->texCoords.clear();
	this->normals.clear();
	this->indices.clear();
	this->tangents.clear();
	this->bitangents.clear();
}

void MeshComponent::update(float deltaTime)
{
	// Make sure the object's VAO is bound
	glBindVertexArray(VAO);

	// Send material data
	if (this->material != nullptr)
		this->material->sendToShader();

	// Send the model matrix
	this->material->shaderProgram
		->setMat4(MeshComponent::MODEL, this->parent->transform->getModelMatrix())
		->setMat3(MeshComponent::NORMAL_MATRIX, this->parent->transform->getNormalMatrix());

	// Indexed drawing
	if (this->hasIndices)
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indicesCount, GL_UNSIGNED_INT, 0);
	else // Normal drawing
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticesCount);
}

void MeshComponent::setupMesh(float vertices[], unsigned int vertSize, Shader* shaderProgram, glm::vec3 position)
{
	this->vertices.insert(this->vertices.end(), &vertices[0], &vertices[vertSize / sizeof(float)]);
	this->parent->transform->setModelMatrix(position, glm::vec3(0), glm::vec3(1.0f));

	this->VAO = {};
	this->VBO = {};
	this->indicesBO = {};
	this->texCoordBO = {};
	this->normalBO = {};

	this->material = std::make_unique<PBRMaterial>(shaderProgram);
}

void MeshComponent::setupMesh(
	std::vector<float> vertices,
	std::vector<float> texCoords,
	std::vector<float> normals,
	std::vector<unsigned int> indices,
	std::vector<std::shared_ptr<Texture>> textures,
	Shader* shaderProgram,
	glm::vec3 position)
{
	this->vertices = vertices;
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

	this->material = std::make_unique<PBRMaterial>(shaderProgram);
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

MeshComponent& MeshComponent::addTexture(std::shared_ptr<Texture> texture)
{
	this->textures.insert(textures.end(), texture);
	return *this;
}

MeshComponent& MeshComponent::addNormals(std::vector<float> normals)
{
	this->normals = normals;
	return *this;
}

// Add normals data to the MeshComponent
MeshComponent& MeshComponent::addNormals(float normals[], unsigned int normalSize)
{
	this->normals.insert(this->normals.end(), &normals[0], &normals[normalSize / sizeof(float)]);
	return *this;
}

MeshComponent& MeshComponent::addIndices(std::vector<unsigned int> indices)
{
	this->indices = indices;
	return *this;
}

// Add indices data to the MeshComponent
MeshComponent& MeshComponent::addIndices(unsigned int indices[], unsigned int indicesSize)
{
	this->indices.insert(this->indices.end(), &indices[0], &indices[indicesSize / sizeof(unsigned int)]);
	return *this;
}

MeshComponent& MeshComponent::addTangents(std::vector<float> tangents)
{
	this->tangents = tangents;
	return *this;
}

MeshComponent& MeshComponent::addBitangents(std::vector<float> bitangents)
{
	this->bitangents = bitangents;
	return *this;
}

int MeshComponent::getIndicesCount()
{
	return this->indicesCount;
}

int MeshComponent::getVerticesCount()
{
	return this->verticesCount;
}

void MeshComponent::setDiffuseColor(glm::vec3 color)
{
	PhongMaterial* phongMaterial = dynamic_cast<PhongMaterial*>(this->material.get());

	if (phongMaterial != nullptr)
	{
		phongMaterial->diffuseColor = color;
		return;
	}

	PBRMaterial* pbrMaterial = dynamic_cast<PBRMaterial*>(this->material.get());

	if (pbrMaterial != nullptr)
		pbrMaterial->albedoColor = color;
}
