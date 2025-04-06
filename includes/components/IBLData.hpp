#pragma once

#include <memory>

#include "cubemap.hpp"
#include "texture.hpp"
#include "renderer.hpp"

/// <summary>
/// Represents a sky and its associated IBL data for PBR rendering
/// </summary>
struct IBLData
{
	/// <summary>
	/// The environment map (skybox)
	/// </summary>
	std::unique_ptr<Cubemap> const environmentMap = std::make_unique<Cubemap>(GL_RGB16F, 512, 512);

	/// <summary>
	/// The irradiance map for diffuse lighting
	/// </summary>
	std::unique_ptr<Cubemap> const irradianceMap = std::make_unique<Cubemap>(GL_RGB16F, 32, 32);

	/// <summary>
	/// The prefiltered map for specular lighting
	/// </summary>
	std::unique_ptr<Cubemap> const prefilterMap = std::make_unique<Cubemap>(GL_RGB16F, 1024, 1024, true);

	// TODO : Make this const
	/// <summary>
	/// The BRDF lookup texture for specular lighting
	/// </summary>
	std::unique_ptr<Texture> brdfLut = nullptr;

	/// <summary>
	/// Generates an environment map and IBL data from a 2D HDR map
	/// </summary>
	/// <param name="renderer">A reference to the renderer</param>
	/// <param name="hdrMap">The HDR map for the sky</param>
	IBLData(Renderer& renderer, std::shared_ptr<Texture> hdrMap);

	/// <summary>
	/// Generates IBL data for an existing cubemap
	/// </summary>
	/// <param name="renderer">A reference to the renderer</param>
	/// <param name="cubemap">The cubemap for the sky</param>
	IBLData(Renderer& renderer, std::unique_ptr<Cubemap> cubemap);

	~IBLData();
};