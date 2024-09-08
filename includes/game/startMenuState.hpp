#pragma once

#include "game/gameState.hpp"
#include "renderer.hpp"

class StartMenuState : public virtual GameState
{
public:
	StartMenuState(Renderer& renderer) : GameState(renderer) {};

	void init() override;
	void cleanup() override;
	void pause() override;
	void resume() override;
	void handleEvents(GameEngine* gameEngine, float deltaTime) override;
	void update(GameEngine* gameEngine, float deltaTime) override;
	void draw(GameEngine* gameEngine, float deltaTime) override;
};