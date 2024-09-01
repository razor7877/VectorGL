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
#include "physics/boundingBox.hpp"

class MeshComponent : public virtual Component
{
public:
	static const std::string MODEL;
	static const std::string NORMAL_MATRIX;

	MeshComponent(Entity* parent);
	~MeshComponent() override;

	void Component::start() override;
	void Component::update(float deltaTime) override;

	/// <summary>
	/// Unlike the update method, this is a much simpler method that only sends model & normal matrices to the material shader before drawing
	/// Much faster to run, used for passes that only need to draw the geometry of the mesh
	/// </summary>
	void drawGeometry();

	/// <summary>
	/// Adds vertices to the mesh
	/// </summary>
	MeshComponent& addVertices(std::vector<float> vertices);

	/// <summary>
	/// Adds vertices to the mesh
	/// </summary>
	MeshComponent& addVertices(float vertices[], unsigned int vertSize);

	/// <summary>
	/// Adds texture coordinates to the mesh
	/// </summary>
	MeshComponent& addTexCoords(std::vector<float> texCoords);

	/// <summary>
	/// Adds texture coordinates to the mesh
	/// </summary>
	MeshComponent& addTexCoords(float texCoords[], unsigned int texSize);

	/// <summary>
	/// Adds normals to the mesh for lighting calculations
	/// </summary>
	MeshComponent& addNormals(std::vector<float> normals);

	/// <summary>
	/// Adds normals to the mesh for lighting calculations
	/// </summary>
	MeshComponent& addNormals(float normals[], unsigned int normalSize);

	/// <summary>
	/// Adds indices to the mesh
	/// </summary>
	MeshComponent& addIndices(std::vector<unsigned int> indices);

	/// <summary>
	/// Adds indices to the mesh
	/// </summary>
	MeshComponent& addIndices(unsigned int indices[], unsigned int indicesSize);

	/// <summary>
	/// Adds tangents to the mesh for normal mapping
	/// </summary>
	MeshComponent& addTangents(std::vector<float> tangents);

	/// <summary>
	/// Adds bitangents to the mesh for normal mapping
	/// </summary>
	MeshComponent& addBitangents(std::vector<float> bitangents);

	/// <summary>
	/// Adds a texture to the mesh
	/// </summary>
	MeshComponent& addTexture(std::shared_ptr<Texture> texture);

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
	int getVerticesCount();

	/// <summary>
	/// Returns how many indices the mesh contains
	/// </summary>
	int getIndicesCount();

	/// <summary>
	/// Returns the bounding box of the object in local space
	/// </summary>
	BoundingBox getLocalBoundingBox();

	/// <summary>
	/// Returns the bounding box of the object in world space
	/// </summary>
	BoundingBox getWorldBoundingBox();

	/// <summary>
	/// Sets the diffuse color used to render the mesh (if no textures are present)
	/// </summary>
	void setDiffuseColor(glm::vec3 color);

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
	int verticesCount = 0;

	/// <summary>
	/// The number of indices stored in the mesh
	/// </summary>
	int indicesCount = 0;

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