#include "game/gameEngine.hpp"
#include "game/gameState.hpp"

GameState* GameEngine::getCurrentState()
{
	if (!this->states.empty())
		return this->states.back();

	return nullptr;
}

void GameEngine::init()
{
	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	this->renderer.init(glm::vec2(10));
}

void GameEngine::cleanup()
{
	while (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}
}

void GameEngine::changeState(GameState* state)
{
	// Cleanup and remove last state if there is one
	if (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}

	this->states.push_back(state);
	state->init();
}

void GameEngine::pushState(GameState* state)
{
	// Pause last state
	if (!this->states.empty())
		this->states.back()->pause();

	this->states.push_back(state);
	state->init();
}

void GameEngine::popState()
{
	// Pop the last state
	if (!this->states.empty())
	{
		this->states.back()->cleanup();
		this->states.pop_back();
	}

	// Resume the one that is now on top
	if (!this->states.empty())
		this->states.back()->resume();
}

void GameEngine::handleEvents(float deltaTime)
{
	this->states.back()->handleEvents(this, deltaTime);
}

void GameEngine::update(float deltaTime)
{
	this->states.back()->update(this, deltaTime);
}

void GameEngine::draw()
{
	this->states.back()->draw(this);
}

bool GameEngine::getIsRunning()
{
	return this->isRunning;
}

void GameEngine::quit()
{
	this->isRunning = false;
}