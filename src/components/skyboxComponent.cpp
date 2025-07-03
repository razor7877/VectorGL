#include <map>

#include "components/skyboxComponent.hpp"
#include "utilities/geometry.hpp"
#include "components/IBLData.hpp"
#include "materials/pbrMaterial.hpp"

SkyboxComponent::SkyboxComponent(Entity* parent) : Component(parent), MeshComponent(parent)
{

}

SkyboxComponent::~SkyboxComponent()
{
	PBRMaterial::irradianceMap = nullptr;
	PBRMaterial::prefilterMap = nullptr;
	PBRMaterial::brdfLut = nullptr;

	for (auto& [type, iblData] : this->skyboxes)
		iblData.reset();
}

void SkyboxComponent::start()
{
	std::vector<float> boxVertices = Geometry::getClockwiseCubeVertices();

	this->setMaterial(std::make_unique<PBRMaterial>(this->shaderProgram))
		.addVertices(boxVertices);
	MeshComponent::start();
}

void SkyboxComponent::update(float deltaTime)
{
	glUseProgram(this->shaderProgram->getID());

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(this->VAO);

	glActiveTexture(GL_TEXTURE0);
	if (this->useIBL)
		this->currentSky->environmentMap->bind();
	else
		this->currentCubemap->bind();

	// Indexed drawing
	if (this->hasIndices)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indicesCount), GL_UNSIGNED_INT, nullptr);
	else // Normal drawing
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->verticesCount));

	glDepthFunc(GL_LESS);
}

void SkyboxComponent::setupSkybox(Shader* shaderProgram, Renderer& renderer)
{
	this->shaderProgram = shaderProgram;

	std::unique_ptr<IBLData> grass = std::make_unique<IBLData>(renderer, std::move(std::make_unique<Cubemap>("img/skybox/grass/")));
	std::unique_ptr<IBLData> night = std::make_unique<IBLData>(renderer, std::move(std::make_unique<Cubemap>("img/skybox/night/")));
	std::unique_ptr<IBLData> sky = std::make_unique<IBLData>(renderer, std::move(std::make_unique<Cubemap>("img/skybox/sky/")));

	this->skyboxes[SkyboxType::GRASS] = std::move(grass);
	this->skyboxes[SkyboxType::NIGHT] = std::move(night);
	this->skyboxes[SkyboxType::SKY] = std::move(sky);

	this->changeSkybox(SkyboxComponent::DEFAULT_SKY);
}

void SkyboxComponent::changeSkybox(SkyboxType sky)
{
	this->currentSky = this->skyboxes[sky].get();
	this->useIBL = true;

	PBRMaterial::irradianceMap = this->currentSky->irradianceMap.get();
	PBRMaterial::prefilterMap = this->currentSky->prefilterMap.get();
	PBRMaterial::brdfLut = this->currentSky->brdfLut.get();
}

void SkyboxComponent::setCubemap(Cubemap* cubemap)
{
	this->currentCubemap = cubemap;
	this->useIBL = false;
}
