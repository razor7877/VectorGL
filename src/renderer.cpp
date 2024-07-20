#include <iostream>
#include <set>
#include <algorithm>

#include <glm/glm/ext/matrix_transform.hpp>

#include "main.hpp"
#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"
#include "materials/pbrMaterial.hpp"

Renderer::Renderer()
{
	// Create the camera and set it up
	std::unique_ptr<Entity> cameraEntity = std::unique_ptr<Entity>(new Entity("Camera"));
	this->currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->addEntity(std::move(cameraEntity));
}

std::vector<Entity*> Renderer::GetEntities()
{
	std::vector<Entity*> rawPointers;

	for (auto&& entity : this->entities)
		rawPointers.push_back(entity.get());

	return rawPointers;
}

GLuint Renderer::GetRenderTexture()
{
	return this->finalTarget.renderTexture;
}

void Renderer::addEntity(std::unique_ptr<Entity> objectPtr)
{
	this->entities.push_back(std::move(objectPtr));
}

void Renderer::removeEntity(std::unique_ptr<Entity> objectPtr)
{
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), objectPtr), this->entities.end());
	objectPtr.reset();
}

void Renderer::removeEntity(Entity* rawObjectPtr)
{
	/*for (auto&& entity : this->entities)
	{
		if (entity.get() == rawObjectPtr)
		{
			this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), entity), this->entities.end());
			entity.reset();
		}
	}*/
	std::vector<std::unique_ptr<Entity>>::iterator entity = std::find_if(this->entities.begin(), this->entities.end(), [&](std::unique_ptr<Entity>& entity) { return entity.get() == rawObjectPtr; });
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), *entity));
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

	// Initializes the light manager if it was setup to send all required data to the shader
	if (LightManager::getInstance().shaderProgram->ID != 0)
		LightManager::getInstance().init();

	for (auto&& entity : this->entities)
		entity->start();

	this->createFramebuffer(windowSize);
}

void Renderer::render(float deltaTime)
{
	// We want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();

	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	// Send light data to shader
	LightManager::getInstance().sendToShader();

	// Render & update the scene
	for (auto&& entity : this->entities)
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
	// Delete all the objects contained in the renderer
	for (auto&& entity : this->entities)
		entity.reset();
}