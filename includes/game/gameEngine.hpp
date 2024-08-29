#pragma once

#include <vector>

#include "renderer.hpp"
#include "components/cameraComponent.hpp"

class GameState;

// Inspired from http://gamedevgeek.com/tutorials/managing-game-states-in-c/

class GameEngine
{
public:
	Renderer renderer = Renderer();
	CameraComponent* cameraComponent = nullptr;

	/// <summary>
	/// Initializes the game
	/// </summary>
	void init();

	/// <summary>
	/// Finishes the game
	/// </summary>
	void cleanup();

	/// <summary>
	/// Changes to a new state
	/// </summary>
	/// <param name="state"></param>
	void changeState(GameState* state);

	/// <summary>
	/// Pushes a new state to the stack
	/// </summary>
	/// <param name="state"></param>
	void pushState(GameState* state);

	/// <summary>
	/// Pops the last state from the stack
	/// </summary>
	void popState();

	/// <summary>
	/// Handles events
	/// </summary>
	void handleEvents(float deltaTime);

	/// <summary>
	/// Game logic
	/// </summary>
	void update(float deltaTime);

	/// <summary>
	/// Render logic
	/// </summary>
	void draw();

	bool getIsRunning();
	void quit();

private:
	std::vector<GameState*> states;
	bool isRunning = false;
};