#pragma once

#include "shader.hpp"

class Material
{
	virtual void sendToShader(Shader* shaderProgram) = 0;
};