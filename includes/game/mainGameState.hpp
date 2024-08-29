#pragma once

#include "game/gameState.hpp"
#include "components/cameraComponent.hpp"

class MainGameState : public virtual GameState
{
public:
	CameraComponent* camera = nullptr;

	MainGameState(Renderer& renderer) : GameState(renderer) {};

	void init() override;
	void cleanup() override;
	void pause() override;
	void resume() override;
	void handleEvents(GameEngine* gameEngine) override;
	void update(GameEngine* gameEngine, float deltaTime) override;
	void draw(GameEngine* gameEngine) override;
};