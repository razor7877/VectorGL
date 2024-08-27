#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	glm::vec3 minPosition = glm::vec3(0.0f);
	glm::vec3 maxPosition = glm::vec3(0.0f);

	BoundingBox() {}
	BoundingBox(glm::vec3 minPosition, glm::vec3 maxPosition) : minPosition(minPosition), maxPosition(maxPosition) {}

	BoundingBox operator*(glm::mat4 modelMatrix)
	{
		BoundingBox result;
		
		float xMin = INFINITY;
		float yMin = INFINITY;
		float zMin = INFINITY;

		float xMax = -INFINITY;
		float yMax = -INFINITY;
		float zMax = -INFINITY;

		std::vector<glm::vec3> corners = {
			this->minPosition,
			glm::vec3(this->minPosition.x, this->minPosition.y, this->maxPosition.z),
			glm::vec3(this->minPosition.x, this->maxPosition.y, this->minPosition.z),
			glm::vec3(this->minPosition.x, this->maxPosition.y, this->maxPosition.z),
			glm::vec3(this->maxPosition.x, this->minPosition.y, this->minPosition.z),
			glm::vec3(this->maxPosition.x, this->minPosition.y, this->maxPosition.z),
			glm::vec3(this->maxPosition.x, this->maxPosition.y, this->minPosition.z),
			this->maxPosition
		};

		for (glm::vec3 corner : corners)
		{
			glm::vec3 worldCorner = glm::vec3(modelMatrix * glm::vec4(corner, 1.0f));

			if (worldCorner.x < xMin) xMin = worldCorner.x;
			if (worldCorner.y < yMin) yMin = worldCorner.y;
			if (worldCorner.z < zMin) zMin = worldCorner.z;

			if (worldCorner.x > xMax) xMax = worldCorner.x;
			if (worldCorner.y > yMax) yMax = worldCorner.y;
			if (worldCorner.z > zMax) zMax = worldCorner.z;
		}

		result.minPosition = glm::vec3(xMin, yMin, zMin);
		result.maxPosition = glm::vec3(xMax, yMax, zMax);

		return result;
	}
};