#include <utilities/glad.h>
#include <glfw/glfw3.h>

#include "game/startMenuState.hpp"
#include "game/mainGameState.hpp"
#include "io/input.hpp"

void StartMenuState::init()
{

}

void StartMenuState::cleanup()
{

}

void StartMenuState::pause()
{

}

void StartMenuState::resume()
{

}

extern GLFWwindow* window;

void StartMenuState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	auto lambda = [this, gameEngine](GLFWwindow* window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			gameEngine->pushState(new MainGameState(this->renderer));
	};

	processInput(lambda, window, deltaTime);
}

void StartMenuState::update(GameEngine* gameEngine, float deltaTime)
{

}

void StartMenuState::draw(GameEngine* gameEngine)
{

}
