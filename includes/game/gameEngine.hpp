#pragma once

#include <vector>
#include <memory>

#include "renderer.hpp"
#include "components/cameraComponent.hpp"

class GameState;

// Inspired from http://gamedevgeek.com/tutorials/managing-game-states-in-c/

class GameEngine
{
public:
	/// <summary>
	/// The renderer shared between all the game states
	/// </summary>
	Renderer renderer = Renderer();

	/// <summary>
	/// Returns the active game state
	/// </summary>
	/// <returns>A pointer to the current state, or nullptr if no state is active</returns>
	std::unique_ptr<GameState>& getCurrentState();

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
	void changeState(std::unique_ptr<GameState> state);

	/// <summary>
	/// Pushes a new state to the stack
	/// </summary>
	/// <param name="state"></param>
	void pushState(std::unique_ptr<GameState> state);

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
	std::vector<std::unique_ptr<GameState>> states;
	bool isRunning = false;
};