#include <map>

#include "components/skyboxComponent.hpp"

// A list of vertices that represent a box used to draw any skybox
float SkyboxComponent::boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

std::map<SkyboxType, Cubemap*> skyboxes =
{
	{ SkyboxType::GRASS, new Cubemap("img/skybox/grass/") },
	{ SkyboxType::NIGHT, new Cubemap("img/skybox/night/") },
	{ SkyboxType::SKY, new Cubemap("img/skybox/sky/") },
};

SkyboxComponent::SkyboxComponent(Entity* parent) : MeshComponent(parent), Component(parent)
{
	this->cubemap = skyboxes[SkyboxComponent::DEFAULT_SKY];
}

void SkyboxComponent::start()
{
	MeshComponent::setupMesh(boxVertices, sizeof(boxVertices), this->shaderProgram);

	MeshComponent::start();

	for (auto& [skyType, cubemap] : skyboxes)
		cubemap->setupObject();
}

void SkyboxComponent::update()
{
	glUseProgram(this->shaderProgram->ID);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(this->VAO);
	this->cubemap->bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void SkyboxComponent::setupSkybox(Shader* shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void SkyboxComponent::changeSkybox(SkyboxType sky)
{
	this->cubemap = skyboxes[sky];
}

void SkyboxComponent::setCubemap(Cubemap* cubemap)
{
	this->cubemap = cubemap;
}
