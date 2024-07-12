#include <iostream>
#include <set>
#include <algorithm>

#include "main.hpp"
#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"

Renderer::Renderer()
{

}

std::vector<Entity*> Renderer::GetEntities()
{
	return this->entities;
}

GLuint Renderer::GetRenderTexture()
{
	return this->finalTarget.renderTexture;
}

void Renderer::addEntity(Entity* objectPtr)
{
	this->entities.push_back(objectPtr);
}

void Renderer::removeEntity(Entity* objectPtr)
{
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), objectPtr), this->entities.end());
}

void Renderer::resizeFramebuffer(glm::vec2 newSize)
{
	this->multiSampledTarget.resize(newSize);
	this->finalTarget.resize(newSize);
}

void Renderer::createFramebuffer(glm::vec2 windowSize)
{
	this->multiSampledTarget = RenderTarget(windowSize, true);
	this->finalTarget = RenderTarget(windowSize, false);
}

void Renderer::init(glm::vec2 windowSize)
{
	this->shaderManager.initUniformBuffer();

	// Create the camera and set it up
	Entity* cameraEntity = new Entity("Camera");
	this->currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->addEntity(cameraEntity);

	// Initializes the light manager if it was setup to send all required data to the shader
	if (LightManager::getInstance().shaderProgram->ID != 0)
		LightManager::getInstance().init();

	for (Entity* entity : this->entities)
		entity->start();

	this->createFramebuffer(windowSize);
}

//void collectAllEntities(Entity* entity, std::vector<Entity*>& allEntities)
//{
//	if (entity == nullptr) return;
//	allEntities.push_back(entity);
//	for (Entity* child : entity->getChildren())
//	{
//		collectAllEntities(child, allEntities);
//	}
//}
//
//std::vector<Entity*> collectAllFromRoots(const std::vector<Entity*>& rootEntities)
//{
//	std::vector<Entity*> allEntities;
//	for (Entity* root : rootEntities)
//	{
//		collectAllEntities(root, allEntities);
//	}
//	return allEntities;
//}

void Renderer::render(float deltaTime)
{
	// We want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();

	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	// Send light data to shader
	LightManager::getInstance().sendToShader();

	//std::vector<Entity*> recursiveEntities = collectAllFromRoots(this->entities);

	//std::vector<std::pair<float, Entity*>> sortedEntities;
	//// We start by storing all the entities in a pair with their distance from the camera
	//for (Entity* entity : recursiveEntities)
	//{
	//	glm::mat4 model = entity->transform->getModelMatrix();
	//	glm::vec3 position = glm::vec3(model[3][0], model[3][1], model[3][2]);

	//	float distance = glm::length(this->currentCamera->getPosition() - position);
	//	sortedEntities.push_back(std::pair(distance, entity));
	//}

	//// Sort the entities by their distance to the camera, to get correct blending
	//std::sort(sortedEntities.begin(), sortedEntities.end(), [](const std::pair<float, Entity*>& a, const std::pair<float, Entity*>& b) { return a.second < b.second; });

	// Render & update the scene
	for (Entity* entity : this->entities)
		entity->update(deltaTime);

	this->multiSampledTarget.unbind();

	// Bind the second target that will contain the mixed multisampled textures
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->multiSampledTarget.framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->finalTarget.framebuffer);

	glm::vec2 framebufferSize = this->multiSampledTarget.size;
	// Resolve the multisampled texture to the second target
	glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::end()
{
	
}