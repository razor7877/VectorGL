#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "component.hpp"
#include "transformComponent.hpp"
#include "texture.hpp"

class MeshComponent : public virtual Component
{
public:
	MeshComponent();
	~MeshComponent();

	void Component::start() override;
	void Component::update() override;

	// Generates buffers and enables correct draw calls to use given texture
	MeshComponent& addTexCoords(std::vector<float> texCoords);
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);
	MeshComponent& addTexture(Texture* texture);
	MeshComponent& addNormals(float normals[], unsigned int normalSize);
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);

protected:
	// The ID of the shader used to draw the object
	GLuint shaderProgramID;

	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	std::vector<Texture*> textures;

	GLuint VAO; // The object's VAO

	GLuint VBO; // The object's VBO
	GLuint texCoordBO;
	GLuint normalBO;
	GLuint indicesBO;
};