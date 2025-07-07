#pragma once

#include <glm/glm.hpp>

#include "materials/Material.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

struct ReflectionMaterial : public virtual Material
{
public:
	explicit ReflectionMaterial(Shader* shaderProgram);
	~ReflectionMaterial() override;

	void init() override;
	void sendToShader() override;
	bool getIsTransparent() override;
	void addTextures(const std::vector<std::shared_ptr<Texture>>& textures) override;
};
