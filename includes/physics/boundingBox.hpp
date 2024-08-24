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
		//result.minPosition = modelMatrix * glm::vec4(this->minPosition, 1.0f);
		//result.maxPosition = modelMatrix * glm::vec4(this->maxPosition, 1.0f);

		glm::vec3 worldMin(FLT_MAX);
		glm::vec3 worldMax(FLT_MIN);

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

		for (const auto& corner : corners)
		{
			glm::vec3 worldCorner = glm::vec3(modelMatrix * glm::vec4(corner, 1.0f));
			worldMin = glm::min(worldMin, worldCorner);
			worldMax = glm::max(worldMax, worldCorner);
		}

		result.minPosition = worldMin;
		result.maxPosition = worldMax;

		return result;
	}
};