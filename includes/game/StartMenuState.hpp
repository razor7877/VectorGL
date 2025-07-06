#pragma once

#include "game/GameState.hpp"
#include "Renderer.hpp"

class StartMenuState : public virtual GameState
{
public:
	explicit StartMenuState(Renderer& renderer) : GameState(renderer) {};

	void init() override;
	void cleanup() override;
	void pause() override;
	void resume() override;
	void handleEvents(GameEngine* gameEngine, float deltaTime) override;
	void update(GameEngine* gameEngine, float deltaTime) override;
	void draw(GameEngine* gameEngine, float deltaTime) override;
};