#pragma once

#include <map>
#include <typeindex>
#include <typeinfo>
#include <string>
#include <vector>

#include "components/component.hpp"
#include "components/transformComponent.hpp"

/// <summary>
/// Represents an entity in the world which can contain various components
/// </summary>
class Entity
{
public:
	TransformComponent* transform;

	Entity();
	Entity(std::string label);
	~Entity();

	/// <summary>
	/// Starts the entity and its components
	/// </summary>
	void start();

	/// <summary>
	/// Updates the entity and its components
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last update</param>
	void update(float deltaTime);

	/// <summary>
	/// Adds a component to the entity if it doesn't already exist
	/// </summary>
	/// <typeparam name="T">The type of the component to create</typeparam>
	template <typename T> T* addComponent();

	/// <summary>
	/// Gets a component from the entity
	/// </summary>
	/// <typeparam name="T">The type of the component</typeparam>
	/// <returns>A pointer to the component, or nullptr it it doesn't have one</returns>
	template <typename T> T* getComponent();

	std::map<std::type_index, Component*> getComponents();

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
	std::map<std::type_index, Component*> components;

	std::string label = "Entity";

	Entity* parent = nullptr;
	std::vector<Entity*> children;

	bool isEnabled = true;
};

template <typename T>
T* Entity::addComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	const std::type_info& componentType = typeid(T);

	if (this->components.count(componentType) == 0)
		this->components[componentType] = new T(this);

	return dynamic_cast<T*>(this->components[componentType]);
}

template <typename T>
T* Entity::getComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	const std::type_info& componentType = typeid(T);

	if (this->components.count(componentType))
		return dynamic_cast<T*>(this->components[componentType]);

	return nullptr;
}