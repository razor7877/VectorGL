#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>

#include "texture.hpp"

class Material
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Texture texture;

	Material();
	Material(Texture texture);
	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, Texture texture);
};

#endif