#pragma once

#include "components/meshComponent.hpp"
#include "cubemap.hpp"

class SkyboxComponent : public MeshComponent
{
public:
	static float boxVertices[];
	
	SkyboxComponent(Entity* parent);

	void Component::start() override;
	void Component::update() override;

	void setupSkybox(Cubemap* cubemap, GLuint shaderProgramID);

private:
	Cubemap* cubemap;
};