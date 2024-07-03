#pragma once

#include <map>
#include <typeinfo>
#include <string>
#include <vector>

#include "components/component.hpp"

/// <summary>
/// Represents an entity in the world which can contain various components
/// </summary>
class Entity
{
public:
	/// <summary>
	/// Updates the entity and its components
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last update</param>
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

	// Returns a label to identify the object in the scene graph
	std::string getLabel();
	// Sets the label of the object
	void setLabel(std::string label);

	// Returns the object's parent
	Entity* getParent();
	// Returns a vector containing the object's children
	std::vector<Entity*> getChildren();
	// Sets the parent of the object
	void setParent(Entity* parent);
	// Adds a child to the object
	void addChild(Entity* child);
	// Removes a child from the object
	void removeChild(Entity* child);

	bool getIsEnabled();
	void setIsEnabled(bool isEnabled);
private:
	std::map<std::type_info, Component*> components;

	std::string label = "Entity";

	Entity* parent;
	std::vector<Entity*> children;

	bool isEnabled;
};