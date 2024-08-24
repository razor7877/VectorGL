#pragma once

#include "physics/plane.hpp"

/// <summary>
/// Represents a frustum
/// </summary>
struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;

	Frustum(CameraComponent* camera, glm::vec2 screenSize)
	{
		float halfVSide = camera->FAR * tanf(glm::radians(camera->getZoom()) * 0.5f);
		float halfHSide = halfVSide * (screenSize.x / screenSize.y);

		glm::vec3 frontMultFar = camera->FAR * camera->getForward();

		glm::vec3 camPos = camera->getPosition();
		glm::vec3 camForward = camera->getForward();
		glm::vec3 camRight = glm::cross(camera->getForward(), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 camUp = glm::cross(camRight, camForward);

		this->nearFace = { camPos + camera->NEAR * camForward, camForward };
		this->farFace = { camPos + frontMultFar, -camForward };
		this->rightFace = { camPos, glm::cross(frontMultFar - camRight * halfHSide, camUp) };
		this->leftFace = { camPos, glm::cross(camUp, frontMultFar + camRight * halfHSide) };
		this->topFace = { camPos, glm::cross(camRight, frontMultFar - camUp * halfVSide) };
		this->bottomFace = { camPos, glm::cross(frontMultFar + camUp * halfVSide, camRight) };
	}
};
