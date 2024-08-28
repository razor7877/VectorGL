#pragma once

#include <glm/glm.hpp>

struct Plane
{
	glm::vec3 normal = { 0.0f, 1.0f, 0.0f };
	float distance = 0.0f;

	Plane() {}
	Plane(glm::vec3 position, glm::vec3 normal)
	{
		this->normal = glm::normalize(normal);
		this->distance = glm::dot(this->normal, position);
	}

	bool isOnOrForwardPlane(glm::vec3 extents, glm::vec3 center)
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r =
			extents.x * std::abs(this->normal.x) +
			extents.y * std::abs(this->normal.y) +
			extents.z * std::abs(this->normal.z);

		return -r <= this->getSignedDistanceToPlane(center);
	}

	float getSignedDistanceToPlane(glm::vec3 point)
	{
		return glm::dot(this->normal, point) - this->distance;
	}
};