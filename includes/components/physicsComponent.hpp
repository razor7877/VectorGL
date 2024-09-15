#pragma once

#include <btBulletDynamicsCommon.h>

#include "components/component.hpp"
#include "physics/collider.hpp"

class PhysicsComponent : public virtual Component
{
public:
	PhysicsComponent(Entity* parent);
	~PhysicsComponent();

	void start() override;
	void update(float deltaTime) override;

	/// <summary>
	/// Sets a collider for the component
	/// </summary>
	/// <param name="world">A reference to the physics world</param>
	/// <param name="rigidBody">The collider to associate with the component</param>
	void setCollider(Collider* collider);

private:
	/// <summary>
	/// The collider of the component
	/// </summary>
	Collider* collider;
};
