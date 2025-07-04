#pragma once

#include <vector>
#include <string>
#include <memory>

#include <utilities/glad.h>
#include <glm/glm.hpp>

#include "materials/material.hpp"
#include "component.hpp"
#include "texture.hpp"
#include "physics/boundingBox.hpp"
#include "shader.hpp"

class MeshComponent : public virtual Component
{
public:
	static const std::string MODEL;
	static const std::string NORMAL_MATRIX;

	explicit MeshComponent(Entity* parent);
	~MeshComponent() override;

	void start() override;
	void update(float deltaTime) override;

	/// <summary>
	/// Unlike the update method, this is a much simpler method that only sends model & normal matrices to the shader before drawing
	/// Much faster to run, used for passes that only need to draw the geometry of the mesh
	/// </summary>
	void drawGeometry(Shader* shaderProgram) const;

	/// <summary>
	/// Adds vertices to the mesh
	/// </summary>
	MeshComponent& addVertices(const std::vector<float> &vertices);

	/// <summary>
	/// Adds vertices to the mesh
	/// </summary>
	MeshComponent& addVertices(float vertices[], unsigned int vertSize);

	/// <summary>
	/// Adds texture coordinates to the mesh
	/// </summary>
	MeshComponent& addTexCoords(const std::vector<float> &texCoords);

	/// <summary>
	/// Adds texture coordinates to the mesh
	/// </summary>
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);

	/// <summary>
	/// Adds normals to the mesh for lighting calculations
	/// </summary>
	MeshComponent& addNormals(const std::vector<float> &normals);

	/// <summary>
	/// Adds normals to the mesh for lighting calculations
	/// </summary>
	MeshComponent& addNormals(float normals[], unsigned int normalSize);

	/// <summary>
	/// Adds indices to the mesh
	/// </summary>
	MeshComponent& addIndices(const std::vector<unsigned int> &indices);

	/// <summary>
	/// Adds indices to the mesh
	/// </summary>
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);

	/// <summary>
	/// Adds tangents to the mesh for normal mapping
	/// </summary>
	MeshComponent& addTangents(const std::vector<float> &tangents);

	/// <summary>
	/// Adds bitangents to the mesh for normal mapping
	/// </summary>
	MeshComponent& addBitangents(const std::vector<float> &bitangents);

	/// <summary>
	/// Adds a texture to the mesh
	/// </summary>
	MeshComponent& addTexture(const std::shared_ptr<Texture>& texture);

	/// <summary>
	/// Adds a list of textures to the mesh
	/// </summary>
	MeshComponent& addTextures(std::vector<std::shared_ptr<Texture>> textures);

	/// <summary>
	/// Sets the material used for rendering the mesh
	/// </summary>
	MeshComponent& setMaterial(std::unique_ptr<Material> material);

	/// <summary>
	/// Returns how many vertices the mesh contains
	/// </summary>
	[[nodiscard]] unsigned long getVerticesCount() const;

	/// <summary>
	/// Returns how many indices the mesh contains
	/// </summary>
	[[nodiscard]] unsigned long getIndicesCount() const;

	/// <summary>
	/// Returns the bounding box of the object in local space
	/// </summary>
	[[nodiscard]] BoundingBox getLocalBoundingBox() const;

	/// <summary>
	/// Returns the bounding box of the object in world space
	/// </summary>
	[[nodiscard]] BoundingBox getWorldBoundingBox();

	/// <summary>
	/// Sets the diffuse color used to render the mesh (if no textures are present)
	/// </summary>
	void setDiffuseColor(glm::vec3 color) const;

	/// <summary>
	/// The material of the mesh
	/// </summary>
	std::unique_ptr<Material> material = nullptr;

protected:
	/// <summary>
	/// The vertices of the mesh
	/// </summary>
	std::vector<float> vertices;

	/// <summary>
	/// The texture coordinates of the mesh
	/// </summary>
	std::vector<float> texCoords;

	/// <summary>
	/// The normals of the mesh for lighting calculations
	/// </summary>
	std::vector<float> normals;

	/// <summary>
	/// The indices of the mesh for indexed drawing
	/// </summary>
	std::vector<unsigned int> indices;

	/// <summary>
	/// The tangents of the mesh for normal mapping
	/// </summary>
	std::vector<float> tangents;

	/// <summary>
	/// The bitangents of the mesh for normal mapping
	/// </summary>
	std::vector<float> bitangents;

	/// <summary>
	/// The number of vertices stored in the mesh
	/// </summary>
	unsigned long verticesCount = 0;

	/// <summary>
	/// The number of indices stored in the mesh
	/// </summary>
	unsigned long indicesCount = 0;

	/// <summary>
	/// Whether the mesh uses indices for drawing
	/// </summary>
	bool hasIndices = false;

	/// <summary>
	/// The list of textures that the mesh contains
	/// </summary>
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

	/// <summary>
	/// The axis aligned bounding box of the mesh in local space
	/// </summary>
	BoundingBox localBoundingBox;

	/// <summary>
	/// The axis aligned bounding box of the mesh in world space
	/// </summary>
	BoundingBox worldBoundingBox;

	/// <summary>
	/// The last global model matrix that was used for calculating the world space AABB
	/// </summary>
	glm::mat4 lastModelMatrix = glm::mat4(0.0f);
};
