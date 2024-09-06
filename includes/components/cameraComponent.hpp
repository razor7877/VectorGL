#pragma once

#include <utilities/glad.h>

#include "entity.hpp"
#include "shaderManager.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class CameraComponent : public virtual Component
{
public:
	// Default camera values
	static constexpr float YAW = -90.0f;
	static constexpr float PITCH = 0.0f;
	static constexpr float SPEED = 7.5f;
	static constexpr float SENSITIVITY = 0.1f;
	static constexpr float ZOOM = 45.0f;
	static constexpr float NEAR = 0.01f;
	static constexpr float FAR = 250.0f;

	CameraComponent(Entity* parent);

	void start() override;
	void update(float deltaTime) override;

	// Returns the view matrix calculated using Euler angles and the lookAt matrix
	glm::mat4 getViewMatrix();
	// Returns the camera's projection matrix depending on zoom
	glm::mat4 getProjectionMatrix(float width, float height);

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void processKeyboard(CameraMovement direction, float deltaTime);
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void processMouseScroll(float yoffset);

	// Getters
	float getZoom();
	float getSpeed();
	float getSensitivity();
	glm::vec3 getPosition();

	// Returns the camera's forward vector
	glm::vec3 getForward();
	glm::vec3 getRight();
	glm::vec3 getUp();

	// Setters
	void setZoom(float zoom);
	void setSpeed(float speed);
	void setSensitivity(float sensitivity);
	void setPosition(glm::vec3 position);

private:
	// Camera attributes
	glm::vec3 front, up, right, worldUp;
	// Camera options
	float movementSpeed, mouseSensitivity, zoom;

	// Calculates the front vector from the camera's (updated) Euler angles
	void updateCameraVectors();
};
