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
	bool drawOutline = false;

	Entity();
	explicit Entity(const std::string &label);
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

	/// <summary>
	/// Returns the map containing the entries for each of the entity's components
	/// </summary>
	/// <returns>A map where each existing entry maps the type_index of a component to the instance of the entity's said component</returns>
	std::map<std::type_index, Component*> getComponents();

	/// <summary>
	/// Returns the entity's transform component
	/// </summary>
	/// <returns>A pointer to the transform component</returns>
	TransformComponent* getTransform() const;

	/// <summary>
	/// Returns a label to identify the object in the scene graph
	/// </summary>
	/// <returns>A string with the entity's label</returns>
	std::string getLabel();

	/// <summary>
	/// Sets the label of the entity
	/// </summary>
	/// <param name="label">The new label for the entity</param>
	void setLabel(const std::string &label);

	/// <summary>
	/// Returns the entity's parent
	/// </summary>
	/// <returns>A pointer to the parent if there is one, or nullptr otherwise</returns>
	Entity* getParent() const;

	/// <summary>
	/// Returns the entity's children
	/// </summary>
	/// <returns>A vector containing pointers to each of the entity's children</returns>
	std::vector<Entity*> getChildren();

	/// <summary>
	/// Sets the parent of the entity
	/// </summary>
	/// <param name="parent">A pointer to the entity to be set as parent</param>
	void setParent(Entity* parent);

	/// <summary>
	/// Adds a child to the entity
	/// </summary>
	/// <param name="child">A pointer to the entity to be added as child</param>
	void addChild(Entity* child);

	/// <summary>
	/// Removes a child from the entity
	/// </summary>
	/// <param name="child">A pointer to the entity to be removed from the children</param>
	void removeChild(Entity* child);

	/// <summary>
	/// Returns whether the entity is currently enabled
	/// </summary>
	/// <returns>True if the entity is enabled, false otherwise</returns>
	bool getIsEnabled() const;

	/// <summary>
	/// Sets whether the entity is currently enabled
	/// </summary>
	/// <param name="isEnabled">Whether the entity should be enabled</param>
	void setIsEnabled(bool isEnabled);

private:
	/// <summary>
	/// A map where each existing entry maps the type_index of a component to the instance of the entity's said component
	/// </summary>
	std::map<std::type_index, Component*> components;

	/// <summary>
	/// A pointer to the entity's transform component, which always exist
	/// </summary>
	TransformComponent* transform = nullptr;

	/// <summary>
	/// A label identifying the entity
	/// </summary>
	std::string label = "Entity";

	/// <summary>
	/// The parent of the entity, if it has one
	/// </summary>
	Entity* parent = nullptr;

	/// <summary>
	/// A list of children of the entity
	/// </summary>
	std::vector<Entity*> children;

	/// <summary>
	/// Whether the entity is currently enabled
	/// </summary>
	bool isEnabled = true;
};

/// <summary>
/// Adds a component to the entity, or returns the existing one if the type already exists
/// </summary>
/// <typeparam name="T">The type of component to create</typeparam>
/// <returns>A pointer to the component</returns>
template <typename T>
T* Entity::addComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

	const std::type_info& componentType = typeid(T);

	if (this->components.count(componentType) == 0)
		this->components[componentType] = new T(this);

	return dynamic_cast<T*>(this->components[componentType]);
}

/// <summary>
/// Tries to get the pointer to a component of the entity
/// </summary>
/// <typeparam name="T">The type of component to query</typeparam>
/// <returns>A pointer to the component, or nullptr it no such component exists</returns>
template <typename T>
T* Entity::getComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

	const std::type_info& componentType = typeid(T);

	if (this->components.count(componentType))
		return dynamic_cast<T*>(this->components[componentType]);

	return nullptr;
}