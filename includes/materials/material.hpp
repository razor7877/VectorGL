#pragma once

#include <vector>

#include "shader.hpp"
#include "texture.hpp"

struct Material
{
	virtual ~Material() {};
	virtual void sendToShader(Shader* shaderProgram) = 0;
	virtual void addTextures(std::vector<std::shared_ptr<Texture>> textures) {}
};