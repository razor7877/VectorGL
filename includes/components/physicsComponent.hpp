#pragma once

#include <btBulletDynamicsCommon.h>

#include "components/component.hpp"

class PhysicsComponent : public virtual Component
{
public:
	PhysicsComponent(Entity* parent);
	~PhysicsComponent();

	void Component::start() override;
	void Component::update(float deltaTime) override;

	/// <summary>
	/// Sets a collider for the component
	/// </summary>
	/// <param name="world">A reference to the physics world</param>
	/// <param name="rigidBody">The collider to associate with the component</param>
	void setCollider(btDiscreteDynamicsWorld* world, btRigidBody* rigidBody);

private:
	/// <summary>
	/// A reference to the physics world 
	/// </summary>
	btDiscreteDynamicsWorld* world = nullptr;

	/// <summary>
	/// The rigid body associated with the component
	/// </summary>
	btRigidBody* rigidBody = nullptr;
};