#include <iostream>
#include <set>
#include <algorithm>

#include <glm/glm/ext/matrix_transform.hpp>

#include "main.hpp"
#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"
#include "materials/pbrMaterial.hpp"
#include "components/meshComponent.hpp"

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

bool Renderer::removeEntity(std::unique_ptr<Entity> objectPtr)
{
	const int startSize = this->entities.size();

	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), objectPtr), this->entities.end());

	// Delete was successful if the size of the vector is different
	return this->entities.size() != startSize;
}

bool Renderer::removeEntity(Entity* rawObjectPtr)
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

void Renderer::resizeFramebuffer(glm::vec2 newSize)
{
	this->multiSampledTarget.bind();
	this->multiSampledTarget.resize(newSize);

	this->finalTarget.bind();
	this->finalTarget.resize(newSize);

	this->finalTarget.unbind();
}

void Renderer::createFramebuffer(glm::vec2 windowSize)
{
	this->multiSampledTarget = RenderTarget(TargetType::TEXTURE_2D_MULTISAMPLE, windowSize);
	this->finalTarget = RenderTarget(TargetType::TEXTURE_2D, windowSize);
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

std::vector<float> lineVerts;
std::vector<float> storedLineVerts;

void Renderer::addLine(glm::vec3 startPos, glm::vec3 endPos, bool store)
{
	if (store)
	{
		storedLineVerts.push_back(startPos.x);
		storedLineVerts.push_back(startPos.y);
		storedLineVerts.push_back(startPos.z);
		storedLineVerts.push_back(endPos.x);
		storedLineVerts.push_back(endPos.y);
		storedLineVerts.push_back(endPos.z);
	}

	lineVerts.push_back(startPos.x);
	lineVerts.push_back(startPos.y);
	lineVerts.push_back(startPos.z);
	lineVerts.push_back(endPos.x);
	lineVerts.push_back(endPos.y);
	lineVerts.push_back(endPos.z);
}

void Renderer::render(float deltaTime)
{
	lineVerts.insert(lineVerts.end(), storedLineVerts.begin(), storedLineVerts.end());

	// We want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();
	this->multiSampledTarget.clear();

	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	// Send light data to shader
	LightManager::getInstance().sendToShader();

	// Update the physics simulation
	this->physicsWorld->update(deltaTime);
	std::vector<float> debugLines = this->physicsWorld->getDebugLines();
	lineVerts.insert(lineVerts.end(), debugLines.begin(), debugLines.end());

	// Render & update the scene
	std::map<MaterialType, std::vector<Entity*>> renderables;
	std::vector<Entity*> nonRenderables;

	for (auto&& entity : this->entities)
	{
		Entity* entityPtr = entity.get();
		MeshComponent* mesh = entityPtr->getComponent<MeshComponent>();

		if (mesh == nullptr)
			nonRenderables.push_back(entityPtr);
		else
			renderables[mesh->material->getType()].push_back(entityPtr);
	}

	for (Entity* nonRenderable : nonRenderables)
		nonRenderable->update(deltaTime);

	for (auto& [material, meshes] : renderables)
	{
		switch (material)
		{
			case MaterialType::PhongMaterial:
				this->shaderManager.getShader(ShaderType::PHONG)->use();
				break;

			case MaterialType::PBRMaterial:
				this->shaderManager.getShader(ShaderType::PBR)->use();
				break;
		}

		for (Entity* renderable : meshes)
			renderable->update(deltaTime);
	}

	if (lineVerts.size() > 0)
	{
		GLuint lineVAO;
		GLuint lineVBO;

		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, lineVerts.size() * sizeof(float), &lineVerts[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		this->shaderManager.getShader(ShaderType::SOLID)->use();
		glBindVertexArray(lineVAO);
		glLineWidth(25.0f);
		glDrawArrays(GL_LINES, 0, lineVerts.size() / 3);
	}

	this->multiSampledTarget.unbind();

	// Bind the second target that will contain the mixed multisampled textures
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->multiSampledTarget.framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->finalTarget.framebuffer);

	glm::vec2 framebufferSize = this->multiSampledTarget.size;
	// Resolve the multisampled texture to the second target
	glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	lineVerts.clear();
}

void Renderer::end()
{
	// Delete all the objects contained in the renderer
	for (auto&& entity : this->entities)
		entity.reset();

	delete this->physicsWorld;
}