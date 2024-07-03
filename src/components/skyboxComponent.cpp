#include "components/skyboxComponent.hpp"

// A list of vertices that represent a box used to draw any skybox
float SkyboxComponent::boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

SkyboxComponent::SkyboxComponent(Entity* parent) : MeshComponent(parent), Component(parent)
{
	this->cubemap = nullptr;
}

void SkyboxComponent::start()
{
	MeshComponent::setupMesh(boxVertices, sizeof(boxVertices), this->shaderProgramID);

	MeshComponent::start();
	cubemap->setupObject();
}

void SkyboxComponent::update()
{
	glUseProgram(this->shaderProgramID);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(this->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texID);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void SkyboxComponent::setupSkybox(Cubemap* cubemap, GLuint shaderProgramID)
{
	this->cubemap = cubemap;
	this->shaderProgramID = shaderProgramID;
}
