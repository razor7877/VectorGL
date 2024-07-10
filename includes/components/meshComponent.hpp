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
	/// <summary>
	/// The diffuse color of the object, used in case it doesn't have a diffuse texture
	/// </summary>
	glm::vec3 diffuseColor = glm::vec3(1.0f);

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
		glm::vec3 position = glm::vec3(0.0f));

	// Generates buffers and enables correct draw calls to use given texture
	MeshComponent& addTexCoords(std::vector<float> texCoords);
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);
	MeshComponent& addTexture(Texture* texture);
	MeshComponent& addNormals(float normals[], unsigned int normalSize);
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);
	MeshComponent& addVertexColors(std::vector<float> vertexColors);

protected:
	/// <summary>
	/// The shader used to draw the mesh
	/// </summary>
	Shader* shaderProgram = nullptr;

	std::vector<float> vertices;
	std::vector<float> vertexColors;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	int verticesCount = 0;
	int indicesCount = 0;
	bool useVertexColors = false;
	bool hasIndices = false;

	std::vector<Texture*> textures;

	/// <summary>
	/// The diffuse texture of the object, if it has one
	/// </summary>
	Texture* diffuseTexture;

	/// <summary>
	/// The specular texture of the object, if it has one
	/// </summary>
	Texture* specularTexture;

	/// <summary>
	/// The normal texture of the object, if it has one
	/// </summary>
	Texture* normalTexture;

	/// <summary>
	/// The height texture of the object, if it has one
	/// </summary>
	Texture* heightTexture;

	bool useDiffuseMap = false;
	bool useSpecularMap = false;
	bool useNormalMap = false;
	bool useHeightMap = false;

	/// <summary>
	/// A OpenGL handle for the vertex array object
	/// </summary>
	GLuint VAO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the vertices
	/// </summary>
	GLuint VBO = 0;

	/// <summary>
	/// A OpenGL handle for the buffer object containing the vertex colors
	/// </summary>
	GLuint vertexColorsBO = 0;

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
};