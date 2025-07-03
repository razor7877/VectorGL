#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

#include "components/cameraComponent.hpp"

CameraComponent::CameraComponent(Entity* parent) : Component(parent)
{
	this->parent->getTransform()->setPosition(glm::vec3(0.0f));
	this->parent->getTransform()->setRotation(YAW, PITCH, 0.0f);

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

void CameraComponent::update(float deltaTime)
{

}

glm::mat4 CameraComponent::getViewMatrix()
{
	this->updateCameraVectors();

	return glm::lookAt(this->parent->getTransform()->getPosition(), this->parent->getTransform()->getPosition() + this->front, this->up);
}

glm::mat4 CameraComponent::getProjectionMatrix(float width, float height) const
{
	return glm::perspective(glm::radians(this->zoom), width / height, NEAR, FAR);
}

void CameraComponent::processKeyboard(CameraMovement direction, float deltaTime) const
{
	if (!this->parent->getIsEnabled())
		return;

	float velocity = movementSpeed * deltaTime;

	TransformComponent* transform = this->parent->getTransform();

	if (direction == CameraMovement::FORWARD)
		transform->setPosition(transform->getPosition() + front * velocity);
	else if (direction == CameraMovement::BACKWARD)
		transform->setPosition(transform->getPosition() - front * velocity);
	else if (direction == CameraMovement::LEFT)
		transform->setPosition(transform->getPosition() - right * velocity);
	else if (direction == CameraMovement::RIGHT)
		transform->setPosition(transform->getPosition() + right * velocity);
}

void CameraComponent::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (!this->parent->getIsEnabled())
		return;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	glm::vec3 rotation = this->parent->getTransform()->getRotation();
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

	this->parent->getTransform()->setRotation(rotation);

	updateCameraVectors();
}

void CameraComponent::processMouseScroll(float yoffset)
{
	if (!this->parent->getIsEnabled())
		return;

	this->zoom -= (float)yoffset;

	if (this->zoom < 1.0f)
		this->zoom = 1.0f;
	if (this->zoom > 90.0f)
		this->zoom = 90.0f;
}

float CameraComponent::getZoom() const
{
	return this->zoom;
}

float CameraComponent::getSpeed() const
{
	return this->movementSpeed;
}

float CameraComponent::getSensitivity() const
{
	return this->mouseSensitivity;
}

glm::vec3 CameraComponent::getPosition() const
{
	return this->parent->getTransform()->getPosition();
}

glm::vec3 CameraComponent::getForward() const
{
	return this->front;
}

glm::vec3 CameraComponent::getRight() const
{
	return this->right;
}

glm::vec3 CameraComponent::getUp() const
{
	return this->up;
}

void CameraComponent::setZoom(const float zoom)
{
	this->zoom = zoom;
}

void CameraComponent::setSpeed(const float speed)
{
	this->movementSpeed = speed;
}

void CameraComponent::setSensitivity(const float sensitivity)
{
	this->mouseSensitivity = sensitivity;
}

void CameraComponent::setPosition(const glm::vec3 position) const {
	this->parent->getTransform()->setPosition(position);
}

void CameraComponent::updateCameraVectors()
{
	glm::vec3 rotation = this->parent->getTransform()->getRotation();

	// Calculate the new front vector
	glm::vec3 newFront{};
	newFront.x = static_cast<float>(cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)));
	newFront.y = static_cast<float>(sin(glm::radians(rotation.y)));
	newFront.z = static_cast<float>(sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)));
	this->front = glm::normalize(newFront);

	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}