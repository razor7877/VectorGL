#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <map>

#include <glad/glad.h>

#include "texture.hpp"
#include "cubemap.hpp"
#include "material.hpp"
#include "renderObject.hpp"

// A mesh is any 3D object that contains vertices data, and may contain normals, textures & materials, indices etc.
// A mesh can exist on it's own or be apart of a Model object
class Mesh : public virtual RenderObject
{
public:
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	std::vector<Texture> textures;

	GLuint VAO; // The object's VAO

	GLuint VBO; // The object's VBO
	GLuint texCoordBO;
	GLuint normalBO;
	GLuint indicesBO;

	glm::mat4 modelMatrix; // The object's model matrix (position in world)

	Mesh();
	// Instantiates a game object, generates the VBO, VAO and attrib pointers
	Mesh(float vertices[], unsigned int vertSize, GLuint shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
	Mesh(std::vector<float> vertices, std::vector<float> texCoords, std::vector<float> normals, std::vector<unsigned int> indices, std::vector<Texture> textures, GLuint shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	// Used in render loop to draw vertices arrays to screen
	void RenderObject::drawObject() override;
	void RenderObject::setupObject() override;

	// Generates buffers and enables correct draw calls to use given texture
	void addTexCoords(std::vector<float> texCoords);
	void addTexCoords(float texCoords[], unsigned int texSize);
	void addTexture(Texture texture);
	void addNormals(float normals[], unsigned int normalSize);
	void addIndices(unsigned int indices[], unsigned int indicesSize);

	// Rotates the object's model matrix using a vec3 or xyz floats
	void rotateMesh(float degrees, glm::vec3 rotationPoint);
	void rotateMesh(float degrees, float x, float y, float z);

	// Translate the object's model matrix using a vec3 or xyz floats
	void translateMesh(glm::vec3 translation);
	void translateMesh(float x, float y, float z);

	// Scales the object's model matrix using a vec3 or xyz floats
	void scaleMesh(glm::vec3 scaleVec);
	void scaleMesh(float scaleX, float scaleY, float scaleZ);
};

#endif