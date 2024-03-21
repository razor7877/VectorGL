#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "renderObject.hpp"
#include "mesh.hpp"

// A class that inherits the Mesh class and overrides the constructor and drawObject() class to
// draw skyboxes
class Skybox : public Mesh
{
public:
	Cubemap* cubemap;
	static float boxVertices[];

	Skybox(GLuint shaderProgramID, Cubemap* cubemap, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	void RenderObject::drawObject() override;
	void RenderObject::setupObject() override;
	std::string RenderObject::getLabel() override;
};

#endif