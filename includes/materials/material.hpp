#pragma once

#include <vector>

#include "shader.hpp"
#include "texture.hpp"

enum class MaterialType
{
	PhongMaterial,
	PBRMaterial,
};

struct Material
{
	/// <summary>
	/// The shader associated with the material
	/// </summary>
	Shader* shaderProgram;

	Material(Shader* shaderProgram) : shaderProgram(shaderProgram) {}
	virtual ~Material() {};

	/// <summary>
	/// Sends the material data to it's corresponding shader
	/// </summary>
	virtual void sendToShader() = 0;

	/// <summary>
	/// Adds a list of textures to the material. Different materials can use them in various ways, or not at all
	/// </summary>
	/// <param name="textures">The textures to be added</param>
	virtual void addTextures(std::vector<std::shared_ptr<Texture>> textures) {}

	/// <summary>
	/// Returns an enum that corresponds to the type of the material
	/// </summary>
	/// <returns></returns>
	virtual MaterialType getType() = 0;
};