#include "materials/ReflectionMaterial.hpp"
#include "Logger.hpp"

ReflectionMaterial::ReflectionMaterial(Shader* shaderProgram) : Material(shaderProgram)
{
	this->ReflectionMaterial::init();
}

ReflectionMaterial::~ReflectionMaterial() = default;

void ReflectionMaterial::init()
{
	
}

void ReflectionMaterial::sendToShader()
{

}

bool ReflectionMaterial::getIsTransparent()
{
	return false;
}

void ReflectionMaterial::addTextures(const std::vector<std::shared_ptr<Texture>>& textures)
{

}
