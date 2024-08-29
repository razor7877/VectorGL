#pragma once

#include "physics/plane.hpp"
#include "components/cameraComponent.hpp"

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
		glm::vec3 camRight = glm::normalize(glm::cross(camera->getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));

		this->nearFace = { camPos + camera->NEAR * camForward, camForward };
		this->farFace = { camPos + frontMultFar, -camForward };
		this->rightFace = { camPos, glm::cross(frontMultFar - camRight * halfHSide, camUp) };
		this->leftFace = { camPos, glm::cross(camUp, frontMultFar + camRight * halfHSide) };
		this->topFace = { camPos, glm::cross(camRight, frontMultFar - camUp * halfVSide) };
		this->bottomFace = { camPos, glm::cross(frontMultFar + camUp * halfVSide, camRight) };
	}

	bool isOnFrustum(BoundingBox element, TransformComponent* transform)
	{
		glm::mat4 globalModelMatrix = transform->getModelMatrix();

		glm::vec3 bbCenter = (element.maxPosition + element.minPosition) * 0.5f;
		glm::vec3 bbExtents = (element.maxPosition - element.minPosition) * 0.5f;

		glm::vec3 globalCenter(globalModelMatrix[3][0], globalModelMatrix[3][1], globalModelMatrix[3][2]);

		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * bbExtents.x;
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f) * bbExtents.y;
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f) * bbExtents.z;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		glm::vec3 newExtents(newIi, newIj, newIk);

		return (this->leftFace.isOnOrForwardPlane(newExtents, globalCenter) &&
			this->rightFace.isOnOrForwardPlane(newExtents, globalCenter) &&
			this->topFace.isOnOrForwardPlane(newExtents, globalCenter) &&
			this->bottomFace.isOnOrForwardPlane(newExtents, globalCenter) &&
			this->nearFace.isOnOrForwardPlane(newExtents, globalCenter) &&
			this->farFace.isOnOrForwardPlane(newExtents, globalCenter));
	}
};
