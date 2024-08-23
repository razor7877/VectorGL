#pragma once

#include <glm/glm.hpp>

struct Plane
{
	glm::vec3 normal = { 0.0f, 1.0f, 0.0f };
	float distance = 0.0f;

	Plane() {}
	Plane(glm::vec3 normal, float distance) : normal(normal), distance(distance) {}
};