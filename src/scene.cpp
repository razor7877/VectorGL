#pragma once

#include "scene.hpp"

std::vector<Entity*> Scene::getEntities()
{
	std::vector<Entity*> rawPointers;

	for (auto&& entity : this->entities)
		rawPointers.push_back(entity.get());

	return rawPointers;
}

void Scene::addEntity(std::unique_ptr<Entity> objectPtr)
{
	this->entities.push_back(std::move(objectPtr));
}

bool Scene::removeEntity(std::unique_ptr<Entity> objectPtr)
{
	const int startSize = this->entities.size();

	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), objectPtr), this->entities.end());

	// Delete was successful if the size of the vector is different
	return this->entities.size() != startSize;
}

bool Scene::removeEntity(Entity* rawObjectPtr)
{
	for (auto&& entity : this->entities)
	{
		if (entity.get() == rawObjectPtr)
		{
			this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), entity), this->entities.end());
			return true;
		}
	}

	return false;
}

void Scene::init()
{
	// Start all the entities on the scene
	for (auto&& entity : this->entities)
		entity->start();
}

void Scene::end()
{
	// Start all the entities on the scene
	for (auto&& entity : this->entities)
		entity.reset();
}

void Scene::getMeshesRecursively(Frustum& cameraFrustum, const std::vector<Entity*>& entities)
{
	// TODO : Don't unnecessarily sort the scene every frame if there are no updates in between
	// TODO : Fix issues with frustum culling when using PhysicsComponent
	// We start by iterating over the entities
	for (Entity* entity : entities)
	{
		// We don't bother iterating over disabled entities
		if (entity->getIsEnabled())
		{
			MeshComponent* mesh = entity->getComponent<MeshComponent>();

			// Check whether we have an entity with a mesh or a purely logic entity
			if (mesh == nullptr)
				this->sortedSceneData.logicEntities.push_back(entity);
			else // Entities that can be rendered are grouped by shader
			{
				this->sortedSceneData.allMeshes.push_back(mesh);

				// Check if the mesh is within the camera frustum
				if (cameraFrustum.isOnFrustum(mesh->getWorldBoundingBox(), mesh->parent->transform))
				{
					this->sortedSceneData.meshes.push_back(mesh);

					PBRMaterial* pbrMat = dynamic_cast<PBRMaterial*>(mesh->material.get());
					if (pbrMat->getIsTransparent())
					{
						float distance = glm::length(this->currentCamera->getPosition() - mesh->getWorldBoundingBox().center);
						this->sortedSceneData.transparentRenderList[pbrMat->shaderProgram][distance] = entity;
					}
					else
						this->sortedSceneData.renderList[mesh->material->shaderProgram].push_back(entity);

					if (entity->drawOutline)
						this->sortedSceneData.outlineRenderList.push_back(entity);
				}
			}

			// Recurse over the children of the entity
			this->getMeshesRecursively(cameraFrustum, entity->getChildren());
		}
	}
}
