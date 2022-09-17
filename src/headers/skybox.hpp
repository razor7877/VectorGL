#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "renderObject.hpp"
#include "mesh.hpp"

// A class that inherits the Mesh class and overrides the constructor and drawObject() class to
// draw skyboxes
class Skybox : public Mesh
{
public:
	Cubemap cubemap;

	Skybox(float vertices[], unsigned int vertSize, GLuint shaderProgramID, Cubemap cubemap, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	void renderObject::drawObject();
};

#endif