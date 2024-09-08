#include "game/gameEngine.hpp"
#include "game/gameState.hpp"
#include "main.hpp"
#include "io/input.hpp"

const std::unique_ptr<GameState>& GameEngine::getCurrentState()
{
	if (!this->states.empty())
		return this->states.back();

	return std::unique_ptr<GameState>(nullptr);
}

void GameEngine::init()
{
	glm::vec2 windowSize = Input::getWindowSize();
	this->renderer.init(glm::vec2(windowSize.x, windowSize.y));
}

void GameEngine::cleanup()
{
	while (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}

	this->renderer.end();
}

void GameEngine::changeState(std::unique_ptr<GameState> state)
{
	// Cleanup and remove last state if there is one
	if (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}

	state->init();
	this->states.push_back(std::move(state));
}

void GameEngine::pushState(std::unique_ptr<GameState> state)
{
	// Pause last state
	if (!this->states.empty())
		this->states.back()->pause();

	// We need to clear the state of all keys to avoid keys being handled by the new state before the release is registered
	Input::clearAllKeys();

	state->init();
	this->states.push_back(std::move(state));
}

void GameEngine::popState()
{
	// Pop the last state
	if (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}
	
	// We need to clear the state of all keys to avoid keys being handled by the previous state before the release is registered
	Input::clearAllKeys();

	// Resume the one that is now on top
	if (!this->states.empty())
		this->states.back()->resume();
}

void GameEngine::handleEvents(float deltaTime)
{
	if (!this->states.empty())
		this->states.back()->handleEvents(this, deltaTime);

	// Processes any mouse or keyboard input for camera movement
	glfwPollEvents();
}

void GameEngine::update(float deltaTime)
{
	if (!this->states.empty())
		this->states.back()->update(this, deltaTime);
}

void GameEngine::draw(float deltaTime)
{
	if (!this->states.empty())
		this->states.back()->draw(this, deltaTime);
}

bool GameEngine::getIsRunning()
{
	return this->isRunning;
}

void GameEngine::quit()
{
	this->isRunning = false;
}
