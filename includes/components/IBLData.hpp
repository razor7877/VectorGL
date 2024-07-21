#pragma once

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
	Cubemap* const environmentMap = new Cubemap(GL_RGB16F, 512, 512);

	/// <summary>
	/// The irradiance map for diffuse lighting
	/// </summary>
	Cubemap* const irradianceMap = new Cubemap(GL_RGB16F, 32, 32);

	/// <summary>
	/// The prefiltered map for specular lighting
	/// </summary>
	Cubemap* const prefilterMap = new Cubemap(GL_RGB16F, 1024, 1024, true);

	/// <summary>
	/// The BRDF lookup texture for specular lighting
	/// </summary>
	Texture* brdfLut = nullptr;

	/// <summary>
	/// Generates an environment map and IBL data from a 2D HDR map
	/// </summary>
	/// <param name="renderer">A reference to the renderer</param>
	/// <param name="hdrMap">The HDR map for the sky</param>
	IBLData(Renderer& renderer, Texture* hdrMap);

	/// <summary>
	/// Generates IBL data for an existing cubemap
	/// </summary>
	/// <param name="renderer">A reference to the renderer</param>
	/// <param name="cubemap">The cubemap for the sky</param>
	IBLData(Renderer& renderer, Cubemap* cubemap);

	~IBLData();
};