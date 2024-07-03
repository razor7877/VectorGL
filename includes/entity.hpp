#pragma once

#include <map>
#include <typeinfo>

#include "components/component.hpp"

class Entity
{
public:
	void update(float deltaTime);

	/// <summary>
	/// Adds a component to the entity if it doesn't already exist
	/// </summary>
	/// <typeparam name="T">The type of the component to create</typeparam>
	template <typename T> void addComponent();

	/// <summary>
	/// Gets a component from the entity
	/// </summary>
	/// <typeparam name="T">The type of the component</typeparam>
	/// <returns>A pointer to the component, or nullptr it it doesn't have one</returns>
	template <typename T> T getComponent();
private:
	std::map<std::type_info, Component*> components;
};