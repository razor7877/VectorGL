#pragma once

#include <glm/glm.hpp>

struct Plane
{
	glm::vec3 normal = { 0.0f, 1.0f, 0.0f };
	glm::vec3 position{};
	float distance = 0.0f;

	Plane() {}
	Plane(glm::vec3 position, glm::vec3 normal) : position(position), normal(normal) {}
};