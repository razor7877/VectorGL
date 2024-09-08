#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include "game/gameState.hpp"
#include "components/cameraComponent.hpp"

class MainGameState : public virtual GameState
{
public:
	btKinematicCharacterController* characterController = nullptr;
	btPairCachingGhostObject* ghostObject = nullptr;
	btConvexShape* characterShape = nullptr;
	
	MainGameState(Renderer& renderer) : GameState(renderer) {};

	void init() override;
	void cleanup() override;
	void pause() override;
	void resume() override;
	void handleEvents(GameEngine* gameEngine, float deltaTime) override;
	void update(GameEngine* gameEngine, float deltaTime) override;
	void draw(GameEngine* gameEngine, float deltaTime) override;
};