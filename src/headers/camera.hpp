#ifndef CAMERA_HPP
#define CAMERA_HPP

// The camera class is responsible for the various actions that can be done by a camera object
// This includes looking around, moving inside the world etc.

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 25.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// Camera attributes
	glm::vec3 position, front, up, right, worldUp;
	// Euler angles
	float yaw, pitch;
	// Camera options
	float movementSpeed, mouseSensitivity, zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		
		updateCameraVectors();
	}

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;

		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler angles and the lookAt matrix
	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void processKeyboard(CameraMovement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == CameraMovement::FORWARD)
		{
			position += front * velocity;
		}
		if (direction == CameraMovement::BACKWARD)
		{
			position -= front * velocity;
		}
		if (direction == CameraMovement::LEFT)
		{
			position -= right * velocity;
		}
		if (direction == CameraMovement::RIGHT)
		{
			position += right * velocity;
		}
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Prevents flipping screen
		if (constrainPitch)
		{
			if (pitch > 89.0f)
			{
				pitch = 89.0f;
			}
			if (pitch < -89.0f)
			{
				pitch = 89.0f;
			}
		}

		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void processMouseScroll(float yoffset)
	{
		zoom -= (float)yoffset;
		if (zoom < 1.0f)
		{
			zoom = 1.0f;
		}
		if (zoom > 90.0f)
		{
			zoom = 90.0f;
		}
	}

private:
	// Calculates the front vector from the camera's (updated) Euler angles
	void updateCameraVectors()
	{
		// Calculate the new front vector
		glm::vec3 newFront{};
		newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(newFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};

#endif