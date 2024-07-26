#pragma once

#include <vector>

#include <utilities/glad.h>
#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "materials/pbrMaterial.hpp"
#include "materials/phongMaterial.hpp"
#include "component.hpp"
#include "transformComponent.hpp"
#include "texture.hpp"
#include "shader.hpp"

class MeshComponent : public virtual Component
{
public:
	static const std::string MODEL;
	static const std::string NORMAL_MATRIX;

	MeshComponent(Entity* parent);
	~MeshComponent() override;

	void Component::start() override;
	void Component::update(float deltaTime) override;

	void setupMesh(float vertices[], unsigned int vertSize, Shader* shaderProgram, glm::vec3 position = glm::vec3(0.0f));
	void setupMesh(
		std::vector<float> vertices,
		std::vector<float> texCoords,
		std::vector<float> normals,
		std::vector<unsigned int> indices,
		std::vector<std::shared_ptr<Texture>> textures,
		Shader* shaderProgram,
		glm::vec3 position = glm::vec3(0.0f));

	// Generates buffers and enables correct draw calls to use given texture
	MeshComponent& addTexCoords(std::vector<float> texCoords);
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);
	MeshComponent& addTexture(std::shared_ptr<Texture> texture);
	MeshComponent& addNormals(std::vector<float> normals);
	MeshComponent& addNormals(float normals[], unsigned int normalSize);
	MeshComponent& addIndices(std::vector<unsigned int> indices);
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);
	MeshComponent& addTangents(std::vector<float> tangents);
	MeshComponent& addBitangents(std::vector<float> bitangents);

	int getVerticesCount();
	int getIndicesCount();

	void setDiffuseColor(glm::vec3 color);

	/// <summary>
	/// The material of the mesh
	/// </summary>
	std::unique_ptr<Material> material = nullptr;

protected:
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
	std::vector<float> tangents;
	std::vector<float> bitangents;

	int verticesCount = 0;
	int indicesCount = 0;
	bool hasIndices = false;

	std::vector<std::shared_ptr<Texture>> textures;

	/// <summary>
	/// A OpenGL handle for the vertex array object
	/// </summary>
	GLuint VAO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the vertices
	/// </summary>
	GLuint VBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the texture coordinates
	/// </summary>
	GLuint texCoordBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the normals
	/// </summary>
	GLuint normalBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the indices
	/// </summary>
	GLuint indicesBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the tangents
	/// </summary>
	GLuint tangentsBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the bitangents
	/// </summary>
	GLuint bitangentsBO = 0;
};