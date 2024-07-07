#include <iostream>

#include "main.hpp"
#include "renderer.hpp"

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

void Renderer::render(float deltaTime)
{
	this->multiSampledTarget.bind();

	glm::vec2 windowSize = this->multiSampledTarget.size;

	// Update camera info
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	for (Entity* entity : this->entities)
		entity->update(deltaTime);

	this->multiSampledTarget.unbind();
}

void Renderer::end()
{
	
}