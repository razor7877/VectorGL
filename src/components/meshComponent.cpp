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
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->texCoordBO);
	glDeleteBuffers(1, &this->normalBO);
	glDeleteBuffers(1, &this->indicesBO);
	glDeleteBuffers(1, &this->tangentsBO);
	glDeleteBuffers(1, &this->bitangentsBO);

	glDeleteVertexArrays(1, &this->VAO);

	this->material.reset();
}

void MeshComponent::start()
{
	// We calculate the normals if none are provided
	if (normals.empty())
	{
		if (!indices.empty())
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
	if (!indices.empty())
	{
		this->hasIndices = true;

		glGenBuffers(1, &indicesBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// If the MeshComponent uses textures, send them to the material
	if (!textures.empty() && this->material != nullptr)
		this->material->addTextures(this->textures);

	// Send texture coordinates if present
	if (!texCoords.empty())
	{
		// Generates a buffer to store texture coordinates data
		glGenBuffers(1, &texCoordBO);

		glBindBuffer(GL_ARRAY_BUFFER, texCoordBO);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(1);
	}
	else if (!textures.empty())
		Logger::logWarning("MeshComponent has texture but no associated texture coordinates!", "meshComponent.cpp");

	// Send normals for lighting calculations
	if (!normals.empty())
	{
		glGenBuffers(1, &normalBO);

		glBindBuffer(GL_ARRAY_BUFFER, normalBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(2);
	}

	// Send tangents for normal mapping
	if (!tangents.empty())
	{
		glGenBuffers(1, &tangentsBO);

		glBindBuffer(GL_ARRAY_BUFFER, tangentsBO);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), &tangents[0], GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(3);
	}

	// Send bitangents for normal mapping
	if (!bitangents.empty())
	{
		glGenBuffers(1, &bitangentsBO);

		glBindBuffer(GL_ARRAY_BUFFER, bitangentsBO);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(float), &bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	}

	this->localBoundingBox = Geometry::getMeshBoundingBox(this->vertices);

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
	{
		if (this->material->shaderProgram->wasRecompiled)
		{
			this->material->init();
			this->material->shaderProgram->wasRecompiled = false;
		}
		
		this->material->sendToShader();

		// Send the model matrix
		this->material->shaderProgram
			->setMat4(MeshComponent::MODEL, this->parent->getTransform()->getModelMatrix())
			->setMat3(MeshComponent::NORMAL_MATRIX, this->parent->getTransform()->getNormalMatrix());
	}

	// Indexed drawing
	if (this->hasIndices)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indicesCount), GL_UNSIGNED_INT, nullptr);
	else // Normal drawing
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->verticesCount));
}

void MeshComponent::drawGeometry(Shader* shaderProgram) const
{
	// Make sure the object's VAO is bound
	glBindVertexArray(VAO);

	// Send only required data for geometry draw
	// Send the model & normal matrices
	shaderProgram
		->setMat4(MeshComponent::MODEL, this->parent->getTransform()->getModelMatrix())
		->setMat3(MeshComponent::NORMAL_MATRIX, this->parent->getTransform()->getNormalMatrix());

	// Indexed drawing
	if (this->hasIndices)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indicesCount), GL_UNSIGNED_INT, nullptr);
	else // Normal drawing
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->verticesCount));
}

MeshComponent& MeshComponent::addVertices(const std::vector<float> &vertices)
{
	this->vertices = vertices;
	return *this;
}

MeshComponent& MeshComponent::addVertices(float vertices[], unsigned int vertSize)
{
	this->vertices.insert(this->vertices.end(), &vertices[0], &vertices[vertSize / sizeof(float)]);
	return *this;
}

MeshComponent& MeshComponent::MeshComponent::addTexCoords(const std::vector<float> &texCoords)
{
	this->texCoords = texCoords;
	return *this;
}

MeshComponent& MeshComponent::addTexCoords(float texCoords[], unsigned int texSize)
{
	this->texCoords.insert(this->texCoords.end(), &texCoords[0], &texCoords[texSize / sizeof(float)]);
	return *this;
}

MeshComponent& MeshComponent::addNormals(const std::vector<float> &normals)
{
	this->normals = normals;
	return *this;
}

MeshComponent& MeshComponent::addNormals(float normals[], unsigned int normalSize)
{
	this->normals.insert(this->normals.end(), &normals[0], &normals[normalSize / sizeof(float)]);
	return *this;
}

MeshComponent& MeshComponent::addIndices(const std::vector<unsigned int> &indices)
{
	this->indices = indices;
	return *this;
}

MeshComponent& MeshComponent::addIndices(unsigned int indices[], unsigned int indicesSize)
{
	this->indices.insert(this->indices.end(), &indices[0], &indices[indicesSize / sizeof(unsigned int)]);
	return *this;
}

MeshComponent& MeshComponent::addTangents(const std::vector<float> &tangents)
{
	this->tangents = tangents;
	return *this;
}

MeshComponent& MeshComponent::addBitangents(const std::vector<float> &bitangents)
{
	this->bitangents = bitangents;
	return *this;
}

MeshComponent& MeshComponent::addTexture(const std::shared_ptr<Texture>& texture)
{
	this->textures.insert(textures.end(), texture);
	return *this;
}

MeshComponent& MeshComponent::addTextures(std::vector<std::shared_ptr<Texture>> textures)
{
	this->textures.insert(this->textures.end(), textures.begin(), textures.end());
	return *this;
}

MeshComponent& MeshComponent::setMaterial(std::unique_ptr<Material> material)
{
	this->material = std::move(material);
	return *this;
}

unsigned long MeshComponent::getVerticesCount() const
{
	return this->verticesCount;
}

unsigned long MeshComponent::getIndicesCount() const
{
	return this->indicesCount;
}

BoundingBox MeshComponent::getLocalBoundingBox() const
{
	return this->localBoundingBox;
}

BoundingBox MeshComponent::getWorldBoundingBox()
{
	glm::mat4 newModelMatrix = this->parent->getTransform()->getModelMatrix();
	
	// We only recalculate the AABB if the matrix changed, since it's fairly expensive to calculate it unnecessarily
	if (this->lastModelMatrix != newModelMatrix)
	{
		this->lastModelMatrix = newModelMatrix;
		this->worldBoundingBox = this->localBoundingBox * lastModelMatrix;
	}

	return this->worldBoundingBox;
}

void MeshComponent::setDiffuseColor(glm::vec3 color) const
{
	// TODO : Pass this to the material instead
	auto* phongMaterial = dynamic_cast<PhongMaterial*>(this->material.get());

	if (phongMaterial != nullptr)
	{
		phongMaterial->diffuseColor = color;
		return;
	}

	auto* pbrMaterial = dynamic_cast<PBRMaterial*>(this->material.get());

	if (pbrMaterial != nullptr)
		pbrMaterial->albedoColor = color;
}
