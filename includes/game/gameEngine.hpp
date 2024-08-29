#pragma once

#include <vector>

#include "game/gameState.hpp"

// Inspired from http://gamedevgeek.com/tutorials/managing-game-states-in-c/

class GameEngine
{
public:
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
	void handleEvents();

	/// <summary>
	/// Game logic
	/// </summary>
	void update();

	/// <summary>
	/// Render logic
	/// </summary>
	void draw();

	bool getIsRunning();
	void setIsRunning();

private:
	std::vector<GameState*> states;
	bool isRunning = false;
};