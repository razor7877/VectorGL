#include <memory>
#include <vector>

#include "game/mainGameState.hpp"
#include "io/input.hpp"

void MainGameState::init()
{
	this->camera = this->renderer.currentCamera;
}

void MainGameState::cleanup()
{
	this->renderer.end();
}

void MainGameState::pause()
{

}

void MainGameState::resume()
{

}

// TODO : Find a better way of handling inputs
extern GLFWwindow* window;

void MainGameState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	CameraComponent* camera = this->camera;
	auto lambda = [camera, gameEngine](GLFWwindow* window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Forward movement
			camera->processKeyboard(CameraMovement::FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Backward movement
			camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Left movement
			camera->processKeyboard(CameraMovement::LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Right movement
			camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
	};

	// Processes any mouse or keyboard input for camera movement
	processInput(lambda, window, deltaTime);
}

void MainGameState::update(GameEngine* gameEngine, float deltaTime)
{
	this->renderer.render(deltaTime);
}

void MainGameState::draw(GameEngine* gameEngine)
{

}
