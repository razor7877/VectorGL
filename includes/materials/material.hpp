#pragma once

#include <vector>
#include <memory>

#include "shader.hpp"
#include "texture.hpp"

struct Material
{
	/// <summary>
	/// The shader associated with the material
	/// </summary>
	Shader* shaderProgram;

	Material(Shader* shaderProgram) : shaderProgram(shaderProgram) {}
	virtual ~Material() {};

	/// <summary>
	/// Initializes the material, this might execute code such as setting one time uniforms (for texture samplers etc.)
	/// Can be called multiple times, for example if shader changes or is recompiled
	/// </summary>
	virtual void init() = 0;

	/// <summary>
	/// Sends the material data to it's corresponding shader
	/// </summary>
	virtual void sendToShader() = 0;

	/// <summary>
	/// Adds a list of textures to the material. Different materials can use them in various ways, or not at all
	/// </summary>
	/// <param name="textures">The textures to be added</param>
	virtual void addTextures(std::vector<std::shared_ptr<Texture>> textures) {}
};