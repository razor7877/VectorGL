#pragma once

#include "renderer.hpp"
#include "scene.hpp"
#include "game/gameEngine.hpp"
#include "physics/physicsWorld.hpp"

class GameState
{
public:
	GameState(Renderer& renderer) : renderer(renderer) {}
	virtual ~GameState() = default;

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
	virtual void handleEvents(GameEngine* gameEngine, float deltaTime) = 0;

	/// <summary>
	/// Game logic
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	virtual void update(GameEngine* gameEngine, float deltaTime) = 0;

	/// <summary>
	/// Render logic
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	virtual void draw(GameEngine* gameEngine, float deltaTime) = 0;

	/// <summary>
	/// Changes to a new state
	/// </summary>
	/// <param name="gameEngine">A reference to the game engine for state changes</param>
	/// <param name="gameState">The new state to change to</param>
	void changeState(GameEngine* gameEngine, std::unique_ptr<GameState> gameState)
	{
		gameEngine->changeState(std::move(gameState));
	}

	/// <summary>
	/// Returns a reference to this state's scene
	/// </summary>
	/// <returns>A reference to a Scene object</returns>
	Scene& getScene()
	{
		return this->scene;
	}

	/// <summary>
	/// Returns a reference to this state's physics world
	/// </summary>
	/// <returns>A reference to a PhysicsWorld object</returns>
	PhysicsWorld& getPhysicsWorld()
	{
		return this->physicsWorld;
	}

protected:
	/// <summary>
	/// A reference to the global renderer
	/// </summary>
	Renderer& renderer;

	/// <summary>
	/// The scene used by the state
	/// </summary>
	Scene scene;

	/// <summary>
	/// The physics world used by the state
	/// </summary>
	PhysicsWorld physicsWorld;
};