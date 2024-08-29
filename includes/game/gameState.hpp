#pragma once

#include "game/gameEngine.hpp"

class GameState
{
public:
	/// <summary>
	/// This is executed when the game state starts
	/// </summary>
	virtual void init() = 0;

	/// <summary>
	/// This is executed when the game state finishes
	/// </summary>
	virtual void cleanup() = 0;

	/// <summary>
	/// This is executed when the game state is paused
	/// </summary>
	virtual void pause() = 0;

	/// <summary>
	/// This is executed when the game state is resumed
	/// </summary>
	virtual void resume() = 0;

	/// <summary>
	/// Handles any events
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	virtual void handleEvents(GameEngine* gameEngine) = 0;

	/// <summary>
	/// Game logic
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	virtual void update(GameEngine* gameEngine) = 0;

	/// <summary>
	/// Render logic
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	virtual void draw(GameEngine* gameEngine) = 0;

	/// <summary>
	/// Changes to a new state
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	/// <param name="gameState">The new state to change to</param>
	void changeState(GameEngine* gameEngine, GameState* gameState)
	{
		gameEngine->changeState(gameState);
	}
};