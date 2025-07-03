#pragma once

#include "components/component.hpp"
#include "physics/collider.hpp"

class PhysicsComponent : public virtual Component
{
public:
	explicit PhysicsComponent(Entity* parent);
	~PhysicsComponent() override;

	void start() override;
	void update(float deltaTime) override;

	/// <summary>
	/// Sets a collider for the component
	/// </summary>
	/// <param name="collider">The collider to associate to the component</param>
	void setCollider(Collider* collider);

private:
	/// <summary>
	/// The collider of the component
	/// </summary>
	Collider* collider = nullptr;
};
