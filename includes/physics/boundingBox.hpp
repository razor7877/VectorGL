#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	/// <summary>
	/// The minimum corner position of the bounding box
	/// </summary>
	glm::vec3 minPosition = glm::vec3(0.0f);

	/// <summary>
	/// The maximum corner position of the bounding box
	/// </summary>
	glm::vec3 maxPosition = glm::vec3(0.0f);

	/// <summary>
	/// The center position of the bounding box
	/// </summary>
	glm::vec3 center = glm::vec3(0.0f);

	BoundingBox() {}

	BoundingBox(glm::vec3 minPosition, glm::vec3 maxPosition) : minPosition(minPosition), maxPosition(maxPosition)
	{
		this->center = (this->minPosition + this->maxPosition) * 0.5f;
	}

	/// <summary>
	/// Applies the transformation of a matrix to the corners of the bounding box (the matrix is first in the multiplication)
	/// </summary>
	/// <param name="modelMatrix">The matrix to multiply the corners with</param>
	/// <returns>A new bounding box with the transformation applied to it</returns>
	BoundingBox operator*(glm::mat4 modelMatrix)
	{
		float xMin = std::numeric_limits<float>::max();
		float yMin = std::numeric_limits<float>::max();
		float zMin = std::numeric_limits<float>::max();

		float xMax = std::numeric_limits<float>::lowest();
		float yMax = std::numeric_limits<float>::lowest();
		float zMax = std::numeric_limits<float>::lowest();

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

		glm::vec3 minPosition = glm::vec3(xMin, yMin, zMin);
		glm::vec3 maxPosition = glm::vec3(xMax, yMax, zMax);

		return BoundingBox(minPosition, maxPosition);
	}
};