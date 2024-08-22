#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	glm::vec3 minPosition = glm::vec3(0.0f);
	glm::vec3 maxPosition = glm::vec3(0.0f);

	BoundingBox() {}
	BoundingBox(glm::vec3 minPosition, glm::vec3 maxPosition) : minPosition(minPosition), maxPosition(maxPosition) {}
};