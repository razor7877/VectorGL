#pragma once

#include <memory>

#include "components/meshComponent.hpp"
#include "cubemap.hpp"
#include "shader.hpp"
#include "components/IBLData.hpp"

enum class SkyboxType
{
	GRASS = 0,
	NIGHT = 1,
	SKY = 2
};

class SkyboxComponent : public MeshComponent
{
public:
	static constexpr SkyboxType DEFAULT_SKY = SkyboxType::SKY;
	
	SkyboxComponent(Entity* parent);
	~SkyboxComponent();

	void start() override;
	void update(float deltaTime) override;

	/// <summary>
	/// Sets up the component before it can be used
	/// </summary>
	/// <param name="shaderProgram">Assigns the shader the component should use to be drawn</param>
	void setupSkybox(Shader* shaderProgram, Renderer& renderer);

	/// <summary>
	/// Changes the skybox cubemap using a SkyboxType
	/// </summary>
	/// <param name="sky">A value of the SkyboxType enum</param>
	void changeSkybox(SkyboxType sky);

	/// <summary>
	/// Manually sets the cubemap used by the skybox
	/// </summary>
	/// <param name="cubemap">A pointer to the cubemap to be used</param>
	void setCubemap(Cubemap* cubemap);

private:
	std::map<SkyboxType, std::unique_ptr<IBLData>> skyboxes;
	bool useIBL = true;

	Shader* shaderProgram = nullptr;
	Cubemap* currentCubemap = nullptr;
	IBLData* currentSky = nullptr;
};
