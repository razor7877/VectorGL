#pragma once

#include <memory>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include "game/gameState.hpp"

class MainGameState : public virtual GameState
{
public:
	std::unique_ptr<btKinematicCharacterController> characterController = nullptr;
	std::unique_ptr<btPairCachingGhostObject> ghostObject = nullptr;
	std::unique_ptr<btConvexShape> characterShape = nullptr;
	
	MainGameState(Renderer& renderer) : GameState(renderer) {};

	void init() override;
	void cleanup() override;
	void pause() override;
	void resume() override;
	void handleEvents(GameEngine* gameEngine, float deltaTime) override;
	void update(GameEngine* gameEngine, float deltaTime) override;
	void draw(GameEngine* gameEngine, float deltaTime) override;
};