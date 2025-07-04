#pragma once

#include <map>
#include <vector>

#include "shader.hpp"
#include "entity.hpp"
#include "components/meshComponent.hpp"
#include "physics/frustum.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/physicsComponent.hpp"

struct SortedSceneData
{
	// Opaque entities that are rendered to the screen
	std::map<Shader*, std::vector<Entity*>> renderList;
	// Transparent entities that are rendered to the screen
	// We use a double map that groups them by shader for performance, then by distance for correct rendering
	std::map<Shader*, std::map<float, Entity*>> transparentRenderList;
	// Entities that should have an outline
	std::vector<Entity*> outlineRenderList;
	// Entities that aren't rendered to the screen but need to be updated
	std::vector<Entity*> logicEntities;
	// The list of all meshes in the scene, for drawing geometry in the shadow or SSAO render passes
	std::vector<MeshComponent*> meshes;
	// All meshes present in the scene, regardless of whether they are inside the camera frustum
	std::vector<MeshComponent*> allMeshes;
	// The list of all physics component that are from entities not inside the camera frustum
	std::vector<PhysicsComponent*> physicsComponents;

	void clearCache()
	{
		renderList.clear();
		transparentRenderList.clear();
		outlineRenderList.clear();
		logicEntities.clear();
		meshes.clear();
		allMeshes.clear();
		physicsComponents.clear();
	}
};

class Scene
{
public:
	/// <summary>
	/// A struct that serves to cache the data obtained when sorting the scene
	/// </summary>
	SortedSceneData sortedSceneData;

	/// <summary>
	/// The current camera used to render the scene
	/// </summary>
	CameraComponent* currentCamera = nullptr;

	/// <summary>
	/// Second camera
	/// </summary>
	CameraComponent* skyCamera = nullptr;

	/// <summary>
	/// The directional light of the scene
	/// </summary>
	DirectionalLightComponent* directionalLight = nullptr;

	/// <summary>
	/// The currently selected entity, if there is one
	/// </summary>
	Entity* currentActiveEntity = nullptr;

	/// <summary>
	/// Returns a list of raw pointers to the entities in the renderer
	/// </summary>
	/// <returns>A vector containing all the raw entities pointers in the renderer</returns>
	std::vector<Entity*> getEntities() const;

	/// <summary>
	/// Adds an entity to the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be added</param>
	void addEntity(std::unique_ptr<Entity> objectPtr);

	/// <summary>
	/// Removes an entity from the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be removed</param>
	/// <returns>True if the entity was successfully removed, false otherwise</returns>
	bool removeEntity(const std::unique_ptr<Entity> &objectPtr);

	/// <summary>
	/// Removes an entity from the renderer using a raw pointer
	/// </summary>
	/// <param name="rawObjectPtr">The raw pointer to the object</param>
	/// <returns>True if the entity was successfully removed, false otherwise</returns>
	bool removeEntity(const Entity* rawObjectPtr);

	/// <summary>
	/// Starts all the entities in the scene
	/// </summary>
	void init() const;

	/// <summary>
	/// Stops all the entities in the scene
	/// </summary>
	void end();

	/// <summary>
	/// Sorts the scene data and stores it into a struct
	/// </summary>
	/// <param name="cameraFrustum">The camera frustum for frustum culling</param>
	/// <param name="entities">The entities to be recursed upon</param>
	void getMeshesRecursively(Frustum& cameraFrustum, const std::vector<Entity*>& entities);

private:
	/// <summary>
	/// The list of entities contained in the scene
	/// </summary>
	std::vector<std::unique_ptr<Entity>> entities;
};
