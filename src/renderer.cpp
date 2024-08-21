#include <iostream>
#include <set>
#include <algorithm>

#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

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

	this->gBuffer.bind();
	this->gBuffer.resize(newSize);

	this->gBuffer.unbind();
}

void Renderer::init(glm::vec2 windowSize)
{
	this->shaderManager.initUniformBuffer();

	// Initializes the light manager if it was setup to send all required data to the shader
	if (LightManager::getInstance().shaderProgram->ID != 0)
		LightManager::getInstance().init();

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::unique_ptr<Entity>(new Entity("Directional light"));
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	this->directionalLight = directionalLightComponent;
	this->addEntity(std::move(dirLightEntity));

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
	// Render & update the scene

	// Entities that are rendered to the screen
	std::map<MaterialType, std::vector<Entity*>> renderables;
	// Entities that should have an outline
	std::vector<Entity*> outlineRenderables;
	std::vector<MeshComponent*> meshes;
	// Entities that aren't rendered to the screen
	std::vector<Entity*> nonRenderables;

	std::vector<Entity*> ptrList;
	for (auto&& entity : this->entities)
		ptrList.push_back(entity.get());

	this->getMeshesRecursively(ptrList, renderables, outlineRenderables, meshes, nonRenderables);

	// Update the physics simulation
	this->physicsWorld->update(deltaTime);

	this->gBuffer.bind();
	this->gBuffer.clear();

	// Use outline shader
	Shader* gBufferShader = this->shaderManager.getShader(ShaderType::GBUFFER);
	gBufferShader->use();

	for (MeshComponent* mesh : meshes)
	{
		Shader* oldShader = mesh->material->shaderProgram;
		mesh->material->shaderProgram = gBufferShader;
		mesh->update(0);
		mesh->material->shaderProgram = oldShader;
	}

	// Render the shadow map
	this->shadowPass(meshes);
	
	// We now want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();
	this->multiSampledTarget.clear();

	// Render the scene
	this->renderPass(deltaTime, renderables, nonRenderables);
	// Render outlines
	this->outlinePass(outlineRenderables);

	this->multiSampledTarget.unbind();

	// Resolve the multisampled framebuffer to the normal one for display
	this->blitPass();
}

void Renderer::end()
{
	// Delete all the objects contained in the renderer
	for (auto&& entity : this->entities)
		entity.reset();

	delete this->physicsWorld;
}

void Renderer::createFramebuffer(glm::vec2 windowSize)
{
	this->multiSampledTarget = RenderTarget(TargetType::TEXTURE_2D_MULTISAMPLE, windowSize);
	this->finalTarget = RenderTarget(TargetType::TEXTURE_2D, windowSize);

	// Shadow mapping
	this->depthMap = RenderTarget(TargetType::TEXTURE_DEPTH, glm::vec2(this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT), GL_DEPTH_COMPONENT);
	PBRMaterial::shadowMap = std::make_shared<Texture>(this->depthMap.renderTexture, TextureType::TEXTURE_ALBEDO);

	// Screen space effects
	this->gBuffer = RenderTarget(TargetType::G_BUFFER, windowSize, GL_RGBA16F);
}

void Renderer::shadowPass(std::vector<MeshComponent*>& meshes)
{
	// We prepare for the depth map rendering for shadow mapping
	this->depthMap.bind();
	this->depthMap.clear();

	glEnable(GL_DEPTH_TEST);

	glm::mat4 dirLightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, this->currentCamera->NEAR, this->currentCamera->FAR);
	glm::mat4 dirLightView = glm::lookAt(this->directionalLight->parent->transform->getPosition(),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = dirLightProjection * dirLightView;
	PBRMaterial::lightSpaceMatrix = lightSpaceMatrix;

	Shader* depthShader = this->shaderManager.getShader(ShaderType::DEPTH);

	depthShader->use()
		->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	for (MeshComponent* mesh : meshes)
	{
		Shader* oldShader = mesh->material->shaderProgram;
		mesh->material->shaderProgram = depthShader;
		mesh->update(0);
		mesh->material->shaderProgram = oldShader;
	}
}

void Renderer::renderPass(float deltaTime, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& nonRenderables)
{
	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));

	// Send light data to shader
	LightManager::getInstance().sendToShader();

	glStencilMask(0x00);

	// We can simply update all entities that won't be rendered
	for (Entity* nonRenderable : nonRenderables)
		nonRenderable->update(deltaTime);

	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	// Entities that can be rendered are grouped by shader and then rendered together
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
		{
			// We only write to the stencil mask if the entity should have an outline
			if (renderable->drawOutline)
				glStencilMask(0xFF);
			else
				glStencilMask(0x00);

			renderable->update(deltaTime);
		}
	}

	// Line drawing for debugging raycasts etc.
	std::vector<float> debugLines = this->physicsWorld->getDebugLines();
	lineVerts.insert(lineVerts.end(), debugLines.begin(), debugLines.end());
	lineVerts.insert(lineVerts.end(), storedLineVerts.begin(), storedLineVerts.end());
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
		lineVerts.clear();
	}

	// Disable stencil writes
	glStencilMask(0x00);
}

void Renderer::outlinePass(std::vector<Entity*>& outlineRenderables)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	// Disable depth test before drawing outlines
	glDisable(GL_DEPTH_TEST);
	// Use outline shader
	Shader* outlineShader = this->shaderManager.getShader(ShaderType::OUTLINE);
	outlineShader->use();

	for (Entity* outlinedEntity : outlineRenderables)
	{
		MeshComponent* mesh = outlinedEntity->getComponent<MeshComponent>();
		glm::vec3 originalScale = outlinedEntity->transform->getScale();
		Shader* originalShader = mesh->material->shaderProgram;

		mesh->material->shaderProgram = outlineShader;

		outlinedEntity->transform->setScale(originalScale * 1.1f);
		outlinedEntity->update(0);
		outlinedEntity->transform->setScale(originalScale);

		mesh->material->shaderProgram = originalShader;
	}

	// Reenable depth test after drawing outlines
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
	// Reenable stencil writes or buffer won't be cleared properly on next frame
	glStencilMask(0xFF);
}

void Renderer::blitPass()
{
	// Bind the second target that will contain the mixed multisampled textures
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->multiSampledTarget.framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->finalTarget.framebuffer);

	glm::vec2 framebufferSize = this->multiSampledTarget.size;
	// Resolve the multisampled texture to the second target
	glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Renderer::getMeshesRecursively(std::vector<Entity*> entities, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& outlineRenderables, std::vector<MeshComponent*>& meshes, std::vector<Entity*>& nonRenderables)
{
	// We start by sorting the entities depending on if they are renderable objects
	for (Entity* entity : entities)
	{
		if (entity->getIsEnabled())
		{
			MeshComponent* mesh = entity->getComponent<MeshComponent>();

			if (mesh == nullptr)
				nonRenderables.push_back(entity);
			else // Entities that can be rendered are grouped by shader
			{
				meshes.push_back(mesh);
				renderables[mesh->material->getType()].push_back(entity);
				if (entity->drawOutline)
					outlineRenderables.push_back(entity);
			}

			this->getMeshesRecursively(entity->getChildren(), renderables, outlineRenderables, meshes, nonRenderables);
		}
	}
}
