#include <map>

#include "components/skyboxComponent.hpp"
#include "shaderManager.hpp"
#include "utilities/geometry.hpp"
#include "components/IBLData.hpp"

SkyboxComponent::SkyboxComponent(Entity* parent) : MeshComponent(parent), Component(parent)
{

}

SkyboxComponent::~SkyboxComponent()
{

}

void SkyboxComponent::start()
{
	std::vector<float> boxVertices = Geometry::getCubeVertices();

	MeshComponent::setupMesh(&boxVertices[0], boxVertices.size() * sizeof(float), this->shaderProgram);
	MeshComponent::start();
}

void SkyboxComponent::update(float deltaTime)
{
	glUseProgram(this->shaderProgram->ID);

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(this->VAO);

	if (this->useIBL)
		this->currentSky->environmentMap->bind();
	else
		this->currentCubemap->bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void SkyboxComponent::setupSkybox(Shader* shaderProgram, Renderer& renderer)
{
	this->shaderProgram = shaderProgram;

	this->skyboxes = {
		{ SkyboxType::GRASS, new IBLData(renderer, new Cubemap("img/skybox/grass/")) },
		{ SkyboxType::NIGHT, new IBLData(renderer, new Cubemap("img/skybox/night/")) },
		{ SkyboxType::SKY, new IBLData(renderer, new Cubemap("img/skybox/sky/")) },
	};

	this->changeSkybox(SkyboxComponent::DEFAULT_SKY);
}

void SkyboxComponent::changeSkybox(SkyboxType sky)
{
	this->currentSky = this->skyboxes[sky];
	this->useIBL = true;

	PBRMaterial::irradianceMap = this->currentSky->irradianceMap;
	PBRMaterial::prefilterMap = this->currentSky->prefilterMap;
	PBRMaterial::brdfLut = this->currentSky->brdfLut;
}

void SkyboxComponent::setCubemap(Cubemap* cubemap)
{
	this->currentCubemap = cubemap;
	this->useIBL = false;
}
