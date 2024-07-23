#pragma once

#include "shader.hpp"

struct Material
{
	virtual void sendToShader(Shader* shaderProgram) = 0;
	virtual ~Material() {};
};