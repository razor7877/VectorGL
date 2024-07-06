#pragma once

#include <vector>

#include <utilities/glad.h>
#include <glm/glm.hpp>

#include "component.hpp"
#include "transformComponent.hpp"
#include "texture.hpp"
#include "shader.hpp"

class MeshComponent : public virtual Component
{
public:
	MeshComponent(Entity* parent);
	~MeshComponent();

	void Component::start() override;
	void Component::update() override;

	void setupMesh(float vertices[], unsigned int vertSize, Shader* shaderProgram, glm::vec3 position = glm::vec3(0));
	void setupMesh(
		std::vector<float> vertices,
		std::vector<float> texCoords,
		std::vector<float> normals,
		std::vector<unsigned int> indices,
		std::vector<Texture*> textures,
		Shader* shaderProgram,
		glm::vec3 position = glm::vec3(1.0f));

	// Generates buffers and enables correct draw calls to use given texture
	MeshComponent& addTexCoords(std::vector<float> texCoords);
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);
	MeshComponent& addTexture(Texture* texture);
	MeshComponent& addNormals(float normals[], unsigned int normalSize);
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);

protected:
	Shader* shaderProgram;

	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	int verticesCount;
	int indicesCount;
	bool hasIndices;

	std::vector<Texture*> textures;

	GLuint VAO;

	// Identifiers for the buffers on GPU
	GLuint VBO;
	GLuint texCoordBO;
	GLuint normalBO;
	GLuint indicesBO;
};