#include <map>

#include "components/skyboxComponent.hpp"
#include "shaderManager.hpp"

// A list of vertices that represent a box used to draw any skybox
float SkyboxComponent::boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

std::map<SkyboxType, Cubemap*> SkyboxComponent::skyboxes =
{
	{ SkyboxType::GRASS, new Cubemap("img/skybox/grass/") },
	{ SkyboxType::NIGHT, new Cubemap("img/skybox/night/") },
	{ SkyboxType::SKY, new Cubemap("img/skybox/sky/") },
};

SkyboxComponent::SkyboxComponent(Entity* parent) : MeshComponent(parent), Component(parent)
{
	this->skyCubemap = this->skyboxes[SkyboxComponent::DEFAULT_SKY];
}

SkyboxComponent::~SkyboxComponent()
{
	this->iblData.release();
}

void SkyboxComponent::start()
{
	MeshComponent::setupMesh(boxVertices, sizeof(boxVertices), this->shaderProgram);

	MeshComponent::start();

	for (auto& [skyType, cubemap] : this->skyboxes)
		cubemap->setupObject();
}

void SkyboxComponent::update()
{
	glUseProgram(this->shaderProgram->ID);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(this->VAO);
	this->skyCubemap->bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void SkyboxComponent::setupSkybox(Shader* shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void SkyboxComponent::changeSkybox(SkyboxType sky)
{
	this->skyCubemap = this->skyboxes[sky];
}

void SkyboxComponent::setCubemap(Cubemap* cubemap)
{
	this->skyCubemap = cubemap;
}
