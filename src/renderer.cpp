#include <iostream>

#include "main.hpp"
#include "renderer.hpp"
#include "renderObject.hpp"

Renderer::Renderer()
{
	this->lightManager = {};

	this->frameBuffer = {};
	this->depthBuffer = {};
	this->renderTexture = {};
}

Renderer::Renderer(GLuint lightShaderProgramID)
{
	this->lightManager = LightManager(lightShaderProgramID);

	this->frameBuffer = {};
	this->depthBuffer = {};
	this->renderTexture = {};
}

std::vector<RenderObject*> Renderer::GetObjects()
{
	return this->objects;
}

std::vector<Entity*> Renderer::GetEntities()
{
	return this->entities;
}

GLuint Renderer::GetRenderTexture()
{
	return this->renderTexture;
}

Renderer& Renderer::addObject(RenderObject* objectPtr)
{
	this->objects.push_back(objectPtr);
	// For each mesh in the renderer's meshes vector, associates the mesh and it's corresponding
	// shader ID using the shaderMap table
	shaderMap[objectPtr->shaderProgramID].push_back(objectPtr);

	objectPtr->setupObject();
	return *this;
}

Renderer& Renderer::removeObject(RenderObject* objectPtr)
{
	// Get the vector associated with the object's shader
	std::vector<RenderObject*>* vector = &this->shaderMap[objectPtr->shaderProgramID];
	vector->erase(std::remove(vector->begin(), vector->end(), objectPtr), vector->end());

	this->objects.erase(std::remove(this->objects.begin(), this->objects.end(), objectPtr), this->objects.end());

	return *this;
}

Renderer& Renderer::addLight(Light* lightPtr)
{
	lightManager.addLight(lightPtr);
	return *this;
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
	if (this->windowSize.x == newSize.x && this->windowSize.y == newSize.y)
		return;

	this->windowSize = newSize;

	glBindTexture(GL_TEXTURE_2D, this->renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->renderTexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);
}

void Renderer::createFramebuffer(glm::vec2 windowSize)
{
	this->windowSize = windowSize;

	glGenFramebuffers(1, &this->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Render texture for the FBO
	glGenTextures(1, &this->renderTexture);
	glBindTexture(GL_TEXTURE_2D, this->renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth buffer for the FBO
	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	// Set texture as color attachment 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderTexture, 0);

	this->drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error while attempting to create framebuffer!" << std::endl;
}

void Renderer::init(glm::vec2 windowSize)
{
	// Initializes the light manager if it was setup to send all required data to the shader
	if (lightManager.shaderProgramID != 0)
	{
		lightManager.init();
	}

	for (Entity* entity : this->entities)
	{
		entity->start();
	}

	this->createFramebuffer(windowSize);
}

void Renderer::render(float deltaTime)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer);
	glViewport(0, 0, this->windowSize.x, this->windowSize.y);

	// Clears the buffers and last frame before rendering the next one
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Iterates over the shaderID : mesh keypair values to draw objects more efficiently
	// Draw calls are grouped by shader
	for (auto& [shader, object] : shaderMap)
	{
		// Use the current shader, then draw all objects associated with it
		glUseProgram(shader);
		for (RenderObject* objectPtr : object)
		{
			if (objectPtr->getIsVisible())
				objectPtr->drawObject();
		}
	}

	for (Entity* entity : this->entities)
		entity->update(deltaTime);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::end()
{
	
}