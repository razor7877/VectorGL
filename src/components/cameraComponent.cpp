#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

#include "components/cameraComponent.hpp"
#include "main.hpp"

CameraComponent::CameraComponent(Entity* parent) : Component(parent)
{
	this->parent->transform->setPosition(glm::vec3(0.0f));
	this->parent->transform->setRotation(YAW, PITCH, 0.0f);

	this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->movementSpeed = SPEED;
	this->mouseSensitivity = SENSITIVITY;
	this->zoom = ZOOM;

	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::vec3(0.0f, 0.0f, 1.0f);
}

void CameraComponent::start()
{

}

void CameraComponent::update()
{
	// Update camera info
	updateUniformBuffer(this->getViewMatrix(), this->getProjectionMatrix(windowWidth, windowHeight));
}

glm::mat4 CameraComponent::getViewMatrix()
{
	this->updateCameraVectors();

	return glm::lookAt(this->parent->transform->getPosition(), this->parent->transform->getPosition() + this->front, this->up);
}

glm::mat4 CameraComponent::getProjectionMatrix(float width, float height)
{
	return glm::perspective(glm::radians(this->zoom), width / height, 0.01f, 100.0f);
}

void CameraComponent::processKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;

	if (direction == CameraMovement::FORWARD)
		this->parent->transform->setPosition(this->parent->transform->getPosition() + front * velocity);
	else if (direction == CameraMovement::BACKWARD)
		this->parent->transform->setPosition(this->parent->transform->getPosition() - front * velocity);
	else if (direction == CameraMovement::LEFT)
		this->parent->transform->setPosition(this->parent->transform->getPosition() - right * velocity);
	else if (direction == CameraMovement::RIGHT)
		this->parent->transform->setPosition(this->parent->transform->getPosition() + right * velocity);
}

void CameraComponent::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	glm::vec3 rotation = this->parent->transform->getRotation();
	rotation.x += xoffset; // Yaw
	rotation.y += yoffset; // Pitch

	// Prevents flipping screen
	if (constrainPitch)
	{
		if (rotation.y > 89.0f)
		{
			rotation.y = 89.0f;
		}
		if (rotation.y < -89.0f)
		{
			rotation.y = -89.0f;
		}
	}

	this->parent->transform->setRotation(rotation);

	updateCameraVectors();
}

void CameraComponent::processMouseScroll(float yoffset)
{
	this->zoom -= (float)yoffset;

	if (this->zoom < 1.0f)
		this->zoom = 1.0f;
	if (this->zoom > 90.0f)
		this->zoom = 90.0f;
}

float CameraComponent::getZoom()
{
	return this->zoom;
}

float CameraComponent::getSpeed()
{
	return this->movementSpeed;
}

float CameraComponent::getSensitivity()
{
	return this->mouseSensitivity;
}

glm::vec3 CameraComponent::getPosition()
{
	return this->parent->transform->getPosition();
}

void CameraComponent::setZoom(float zoom)
{
	this->zoom = zoom;
}

void CameraComponent::setSpeed(float speed)
{
	this->movementSpeed = speed;
}

void CameraComponent::setSensitivity(float sensitivity)
{
	this->mouseSensitivity = sensitivity;
}

void CameraComponent::setPosition(glm::vec3 position)
{
	this->parent->transform->setPosition(position);
}

void CameraComponent::updateCameraVectors()
{
	glm::vec3 rotation = this->parent->transform->getRotation();

	// Calculate the new front vector
	glm::vec3 newFront{};
	newFront.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	newFront.y = sin(glm::radians(rotation.y));
	newFront.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	this->front = glm::normalize(newFront);

	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}