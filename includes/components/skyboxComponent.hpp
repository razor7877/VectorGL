#pragma once

#include "components/meshComponent.hpp"
#include "cubemap.hpp"

enum class SkyboxType
{
	GRASS = 0,
	NIGHT = 1,
	SKY = 2
};

class SkyboxComponent : public MeshComponent
{
public:
	static const SkyboxType DEFAULT_SKY = SkyboxType::NIGHT;

	static float boxVertices[];
	
	SkyboxComponent(Entity* parent);

	void Component::start() override;
	void Component::update() override;

	void setupSkybox(GLuint shaderProgramID);
	void changeSkybox(SkyboxType sky);

private:
	Cubemap* cubemap;
};