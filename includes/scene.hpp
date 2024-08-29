#pragma once

#include <map>
#include <vector>

#include "shader.hpp"
#include "entity.hpp"
#include "components/meshComponent.hpp"
#include "physics/frustum.hpp"
#include "components/lights/directionalLightComponent.hpp"

struct SortedSceneData
{
	// Opaque entities that are rendered to the screen
	std::map<Shader*, std::vector<Entity*>> renderList;
	// Transparent entities that are rendered to the screen
	std::map<Shader*, std::vector<Entity*>> transparentRenderList;
	// Entities that should have an outline
	std::vector<Entity*> outlineRenderList;
	// Entities that aren't rendered to the screen but need to be updated
	std::vector<Entity*> logicEntities;
	// The list of all meshes in the scene, for drawing geometry in the shadow or SSAO render passes
	std::vector<MeshComponent*> meshes;
	std::vector<MeshComponent*> allMeshes;

	void clearCache()
	{
		renderList.clear();
		transparentRenderList.clear();
		outlineRenderList.clear();
		logicEntities.clear();
		meshes.clear();
		allMeshes.clear();
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
	CameraComponent* currentCamera;

	CameraComponent* skyCamera;

	DirectionalLightComponent* directionalLight;

	/// <summary>
	/// Returns a list of raw pointers to the entities in the renderer
	/// </summary>
	/// <returns>A vector containing all the raw entities pointers in the renderer</returns>
	std::vector<Entity*> getEntities();

	/// <summary>
	/// Adds an entity to the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be added</param>
	void addEntity(std::unique_ptr<Entity> objectPtr);

	/// <summary>
	/// Removes an entity from the renderer
	/// </summary>
	/// <param name="objectPtr">The entity to be removed</param>
	/// <returns>True if the entity was successfully removed, false otherwise</return>
	bool removeEntity(std::unique_ptr<Entity> objectPtr);

	/// <summary>
	/// Removes an entity from the renderer using a raw pointer
	/// </summary>
	/// <param name="rawObjectPtr">The raw pointer to the object</param>
	/// <returns>True if the entity was successfully removed, false otherwise</returns>
	bool removeEntity(Entity* rawObjectPtr);

	/// <summary>
	/// Starts all the entities in the scene
	/// </summary>
	void init();

	/// <summary>
	/// Stops all the entities in the scene
	/// </summary>
	void end();

	/// <summary>
	/// Sorts the scene data and stores it into a struct
	/// </summary>
	/// <param name="cameraFrustum">The camera frustum for frustum culling</param>
	/// <param name="entities">The entities to be recursed upon</param>
	void getMeshesRecursively(Frustum& cameraFrustum, std::vector<Entity*>& entities);

private:
	/// <summary>
	/// The list of entities contained in the scene
	/// </summary>
	std::vector<std::unique_ptr<Entity>> entities;
};