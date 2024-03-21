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
	Mesh();
	// Instantiates a game object, generates the VBO, VAO and attrib pointers
	Mesh(float vertices[], unsigned int vertSize, GLuint shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
	Mesh(std::vector<float> vertices, std::vector<float> texCoords, std::vector<float> normals, std::vector<unsigned int> indices, std::vector<Texture*> textures, GLuint shaderProgramID, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
	~Mesh() override;

	// Used in render loop to draw vertices arrays to screen
	void RenderObject::drawObject() override;
	void RenderObject::setupObject() override;
	std::string RenderObject::getLabel() override;

	// Generates buffers and enables correct draw calls to use given texture
	Mesh& addTexCoords(std::vector<float> texCoords);
	Mesh& addTexCoords(float texCoords[], unsigned int texSize);
	Mesh& addTexture(Texture* texture);
	Mesh& addNormals(float normals[], unsigned int normalSize);
	Mesh& addIndices(unsigned int indices[], unsigned int indicesSize);

protected:
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

#endif