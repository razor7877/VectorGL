#pragma once

#include <memory>

#include <btBulletDynamicsCommon.h>

/// <summary>
/// A wrapper struct that holds all the Bullet objects that need to be managed for a collider
/// </summary>
struct Collider
{
	btDiscreteDynamicsWorld* world = nullptr;
	std::unique_ptr<btCollisionShape> collisionShape = nullptr;
	std::unique_ptr<btDefaultMotionState> motionState = nullptr;
	std::unique_ptr<btRigidBody> rigidBody = nullptr;

	Collider(btDiscreteDynamicsWorld* world, std::unique_ptr<btCollisionShape> collisionShape, std::unique_ptr<btDefaultMotionState> motionState, std::unique_ptr<btRigidBody> rigidBody)
	{
		this->world = world;
		this->collisionShape = std::move(collisionShape);
		this->motionState = std::move(motionState);
		this->rigidBody = std::move(rigidBody);
	}
};
