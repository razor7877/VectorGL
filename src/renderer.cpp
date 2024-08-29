#include <iostream>
#include <set>
#include <algorithm>
#include <random>

#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

#include "main.hpp"
#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"
#include "materials/pbrMaterial.hpp"
#include "components/meshComponent.hpp"
#include "utilities/geometry.hpp"
#include "utilities/geometry.hpp"
#include "physics/frustum.hpp"

Renderer::Renderer()
{
	
}

std::vector<Entity*> Renderer::getEntities()
{
	std::vector<Entity*> rawPointers;

	for (auto&& entity : this->entities)
		rawPointers.push_back(entity.get());

	return rawPointers;
}

GLuint Renderer::getRenderTexture()
{
	return this->finalTarget.renderTexture;
}

GLuint Renderer::getSkyRenderTexture()
{
	return this->skyTarget.renderTexture;
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

void Renderer::resizeFramebuffers(glm::vec2 newSize)
{
	this->multiSampledTarget.bind();
	this->multiSampledTarget.resize(newSize);

	this->finalTarget.bind();
	this->finalTarget.resize(newSize);

	this->gBuffer.bind();
	this->gBuffer.resize(newSize);

	this->ssaoTarget.bind();
	this->ssaoTarget.resize(newSize * this->SSAO_SCALE_FACTOR);

	this->ssaoBlurTarget.bind();
	this->ssaoBlurTarget.resize(newSize * this->SSAO_SCALE_FACTOR);

	this->ssaoBlurTarget.unbind();
}


void Renderer::init(glm::vec2 windowSize)
{
	// Create the camera and set it up
	std::unique_ptr<Entity> cameraEntity = std::unique_ptr<Entity>(new Entity("Camera"));
	MeshComponent* cameraMesh = cameraEntity->addComponent<MeshComponent>();
	VertexData sphere = Geometry::getSphereVertices(100, 30);
	VertexDataIndices sphereOptimized = Geometry::optimizeVertices(sphere.vertices, sphere.normals);
	cameraMesh->setMaterial(std::make_unique<PBRMaterial>(this->shaderManager.getShader(ShaderType::PBR)))
		.addVertices(sphereOptimized.vertices)
		.addIndices(sphereOptimized.indices)
		.addNormals(sphereOptimized.normals)
		.setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
	this->currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->addEntity(std::move(cameraEntity));

	std::unique_ptr<Entity> skyCameraEntity = std::unique_ptr<Entity>(new Entity("Sky Camera"));
	this->skyCamera = skyCameraEntity->addComponent<CameraComponent>();
	this->skyCamera->setPosition(glm::vec3(0.0f, 75.0f, 0.0f));
	this->skyCamera->setZoom(90.0f);
	skyCameraEntity->transform->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
	this->addEntity(std::move(skyCameraEntity));

	this->shaderManager.initUniformBuffer();

	// Initializes the light manager if it was setup to send all required data to the shader
	if (LightManager::getInstance().shaderProgram->ID != 0)
		LightManager::getInstance().init();

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::unique_ptr<Entity>(new Entity("Directional light"));
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	this->directionalLight = directionalLightComponent;
	this->addEntity(std::move(dirLightEntity));

	// Start all the entities on the scene
	for (auto&& entity : this->entities)
		entity->start();

	// Initialize SSAO kernels
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0f - 1.0f, // x between -1:1
			randomFloats(generator) * 2.0f - 1.0f, // y between -1:1
			randomFloats(generator) // z between 0:1
		);

		sample = glm::normalize(sample);

		float scale = (float)i / 64.0f;
		// Lerp
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
		sample *= scale;

		ssaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f
		);

		ssaoNoise.push_back(noise);
	}

	GLuint noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	this->ssaoNoiseTexture = std::make_unique<Texture>(noiseTexture, TextureType::TEXTURE_ALBEDO);

	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	this->ssaoQuad = std::make_unique<Entity>("Quad");
	MeshComponent* ssaoQuadMesh = ssaoQuad->addComponent<MeshComponent>();
	ssaoQuadMesh->setMaterial(std::make_unique<PBRMaterial>(this->shaderManager.getShader(ShaderType::SSAO)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoQuadMesh->start();

	this->ssaoBlurQuad = std::make_unique<Entity>("Quad");
	MeshComponent* ssaoBlurQuadMesh = ssaoBlurQuad->addComponent<MeshComponent>();
	ssaoBlurQuadMesh->setMaterial(std::make_unique<PBRMaterial>(this->shaderManager.getShader(ShaderType::SSAOBLUR)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoBlurQuadMesh->start();

	this->createFramebuffers(windowSize);
}

void Renderer::addLine(glm::vec3 startPos, glm::vec3 endPos, bool store)
{
	if (store)
	{
		this->storedLineVerts.push_back(startPos.x);
		this->storedLineVerts.push_back(startPos.y);
		this->storedLineVerts.push_back(startPos.z);
		this->storedLineVerts.push_back(endPos.x);
		this->storedLineVerts.push_back(endPos.y);
		this->storedLineVerts.push_back(endPos.z);
	}

	this->lineVerts.push_back(startPos.x);
	this->lineVerts.push_back(startPos.y);
	this->lineVerts.push_back(startPos.z);
	this->lineVerts.push_back(endPos.x);
	this->lineVerts.push_back(endPos.y);
	this->lineVerts.push_back(endPos.z);
}

void Renderer::render(float deltaTime)
{
	double frameStartTime = glfwGetTime();

	// Render & update the scene

	// All the entities at the top level of the scene
	std::vector<Entity*> entities;

	for (auto&& entity : this->entities)
		entities.push_back(entity.get());

	double startTime = glfwGetTime();
	this->sortedSceneData.clearCache();
	Frustum frustum(this->currentCamera, this->multiSampledTarget.size);
	this->getMeshesRecursively(frustum, entities, this->sortedSceneData);
	double endTime = glfwGetTime();
	this->meshSortingTime = endTime - startTime;

	// Update the physics simulation
	startTime = glfwGetTime();
	this->physicsWorld->update(deltaTime);
	endTime = glfwGetTime();
	this->physicsUpdateTime = endTime - startTime;

	// Update camera info
	glm::vec2 windowSize = this->multiSampledTarget.size;
	this->shaderManager.updateUniformBuffer(this->currentCamera->getViewMatrix(), this->currentCamera->getProjectionMatrix(windowSize.x, windowSize.y));
	// Send light data to shader
	LightManager::getInstance().sendToShader();

	// Render the shadow map
	startTime = glfwGetTime();
	this->shadowPass(sortedSceneData.meshes);
	endTime = glfwGetTime();
	this->shadowPassTime = endTime - startTime;

	// Render to the G buffer
	startTime = glfwGetTime();
	this->gBufferPass(sortedSceneData.meshes);
	endTime = glfwGetTime();
	this->gBufferPassTime = endTime - startTime;

	// Calculate SSAO
	startTime = glfwGetTime();
	this->ssaoPass(sortedSceneData.meshes);
	endTime = glfwGetTime();
	this->ssaoPassTime = endTime - startTime;
	
	// We now want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();
	this->multiSampledTarget.clear();

	// Render the scene
	startTime = glfwGetTime();
	this->renderPass(
		deltaTime,
		sortedSceneData.renderList,
		sortedSceneData.transparentRenderList,
		sortedSceneData.logicEntities,
		sortedSceneData.meshes
	);
	endTime = glfwGetTime();
	this->renderPassTime = endTime - startTime;

	// Render outlines
	startTime = glfwGetTime();
	this->outlinePass(sortedSceneData.outlineRenderList);
	endTime = glfwGetTime();
	this->outlinePassTime = endTime - startTime;

	this->multiSampledTarget.unbind();

	// Resolve the multisampled framebuffer to the normal one for display
	startTime = glfwGetTime();
	this->blitPass();
	endTime = glfwGetTime();
	this->blitPassTime = endTime - startTime;

	this->frameRenderTime = glfwGetTime() - frameStartTime;

	this->skyTarget.bind();
	this->skyTarget.clear();

	/*this->shaderManager.updateUniformBuffer(this->skyCamera->getViewMatrix(), this->skyCamera->getProjectionMatrix(windowSize.x, windowSize.y));
	this->renderPass(deltaTime, renderList, transparentRenderList, logicEntities, meshes);*/

	this->skyTarget.unbind();
}

void Renderer::end()
{
	// Delete all the objects contained in the renderer
	for (auto&& entity : this->entities)
		entity.reset();

	delete this->physicsWorld;
}

void Renderer::createFramebuffers(glm::vec2 windowSize)
{
	this->multiSampledTarget = RenderTarget(TargetType::TEXTURE_2D_MULTISAMPLE, windowSize);
	this->finalTarget = RenderTarget(TargetType::TEXTURE_2D, windowSize);
	this->skyTarget = RenderTarget(TargetType::TEXTURE_2D, windowSize);

	// Shadow mapping
	this->depthMap = RenderTarget(TargetType::TEXTURE_DEPTH, glm::vec2(this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT), GL_DEPTH_COMPONENT);
	PBRMaterial::shadowMap = std::make_shared<Texture>(this->depthMap.renderTexture, TextureType::TEXTURE_ALBEDO);

	// Screen space effects
	this->gBuffer = RenderTarget(TargetType::G_BUFFER, windowSize, GL_RGBA16F);
	this->ssaoTarget = RenderTarget(TargetType::TEXTURE_RED, windowSize * this->SSAO_SCALE_FACTOR, GL_RED);
	this->ssaoBlurTarget = RenderTarget(TargetType::TEXTURE_RED, windowSize * this->SSAO_SCALE_FACTOR, GL_RED);
	PBRMaterial::ssaoMap = std::make_shared<Texture>(this->ssaoBlurTarget.renderTexture, TextureType::TEXTURE_ALBEDO);
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
		mesh->drawGeometry();
		mesh->material->shaderProgram = oldShader;
	}

	this->depthMap.unbind();
}

void Renderer::gBufferPass(std::vector<MeshComponent*>& meshes)
{
	this->gBuffer.bind();
	this->gBuffer.clear();

	// Use G-buffer shader
	Shader* gBufferShader = this->shaderManager.getShader(ShaderType::GBUFFER);
	gBufferShader->use();

	for (MeshComponent* mesh : meshes)
	{
		Shader* oldShader = mesh->material->shaderProgram;
		mesh->material->shaderProgram = gBufferShader;
		mesh->drawGeometry();
		mesh->material->shaderProgram = oldShader;
	}

	this->gBuffer.unbind();
}

void Renderer::ssaoPass(std::vector<MeshComponent*>& meshes)
{
	this->ssaoTarget.bind();
	this->ssaoTarget.clear();

	Shader* ssaoShader = this->shaderManager.getShader(ShaderType::SSAO);
	// Setup required uniforms
	ssaoShader->use()
		->setInt("gPosition", 0)
		->setInt("gNormal", 1)
		->setInt("texNoise", 2)
		->setVec2("noiseScale", glm::vec2(this->ssaoTarget.size.x / 4.0f, this->ssaoTarget.size.y / 4.0f));

	for (unsigned int i = 0; i < 64; i++)
		ssaoShader->setVec3("samples[" + std::to_string(i) + "]", this->ssaoKernel[i]);

	// Bind the G buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gBuffer.gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->gBuffer.gNormal);

	glActiveTexture(GL_TEXTURE2);
	this->ssaoNoiseTexture->bindTexture();

	// Render SSAO to quad
	this->ssaoQuad->update(0);

	this->ssaoTarget.unbind();

	// Now we want to blur the result to correct the repeating noise pattern
	this->ssaoBlurTarget.bind();
	this->ssaoBlurTarget.clear();

	Shader* ssaoBlurShader = this->shaderManager.getShader(ShaderType::SSAOBLUR);
	ssaoBlurShader->use()
		->setInt("ssaoInput", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->ssaoTarget.renderTexture);

	this->ssaoBlurQuad->update(0);

	this->ssaoBlurTarget.unbind();
}

void Renderer::renderPass(
	float deltaTime,
	std::map<Shader*, std::vector<Entity*>>& renderables,
	std::map<Shader*, std::vector<Entity*>>& transparentRenderables,
	std::vector<Entity*>& nonRenderables,
	std::vector<MeshComponent*> meshes)
{
	glStencilMask(0x00);

	// We can simply update all entities that won't be rendered
	for (Entity* nonRenderable : nonRenderables)
		nonRenderable->update(deltaTime);

	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	this->shaderManager.getShader(ShaderType::PBR)
		->use()
		->setVec2("windowSize", this->ssaoTarget.size);

	// Entities that can be rendered are grouped by shader and then rendered together
	for (auto& [shader, meshes] : renderables)
	{
		shader->use();

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

	for (auto& [shader, meshes] : transparentRenderables)
	{
		shader->use();

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

	if (this->enableDebugDraw)
	{
		// Line drawing for debugging raycasts etc.
		std::vector<float> debugLines = this->physicsWorld->getDebugLines();
		lineVerts.insert(lineVerts.end(), debugLines.begin(), debugLines.end());
		lineVerts.insert(lineVerts.end(), storedLineVerts.begin(), storedLineVerts.end());

		// Debug bounding boxes
		for (MeshComponent* mesh : meshes)
		{
			std::vector<float> vertices;

			// Querying the bounding boxes like that every frame is super slow, could probably be improved
			BoundingBox meshBB = mesh->getWorldBoundingBox();
			glm::vec3 minPos = meshBB.minPosition;
			glm::vec3 maxPos = meshBB.maxPosition;

			// Create the vertices for the 8 points of the bounding box
			// Left bottom back
			vertices.push_back(minPos[0]); vertices.push_back(minPos[1]); vertices.push_back(minPos[2]); // (x_min, y_min, z_min)
			// Left bottom front
			vertices.push_back(minPos[0]); vertices.push_back(minPos[1]); vertices.push_back(maxPos[2]); // (x_min, y_min, z_max)
			// Left top back
			vertices.push_back(minPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(minPos[2]); // (x_min, y_max, z_min)
			// Left top front
			vertices.push_back(minPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(maxPos[2]); // (x_min, y_max, z_max)
			// Right bottom back
			vertices.push_back(maxPos[0]); vertices.push_back(minPos[1]); vertices.push_back(minPos[2]); // (x_max, y_min, z_min)
			// Right bottom front
			vertices.push_back(maxPos[0]); vertices.push_back(minPos[1]); vertices.push_back(maxPos[2]); // (x_max, y_min, z_max)
			// Right top back
			vertices.push_back(maxPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(minPos[2]); // (x_max, y_max, z_min)
			// Right top front
			vertices.push_back(maxPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(maxPos[2]); // (x_max, y_max, z_max)

			// The indices for creating lines that links all the points of the bounding box using the 8 previous vertices
			int edgeIndices[12][2] = {
				{0, 1}, {0, 2}, {1, 3}, {2, 3}, // Left side edges
				{4, 5}, {4, 6}, {5, 7}, {6, 7}, // Right side edges
				{0, 4}, {2, 6}, {1, 5}, {3, 7}, // Connect the two sides
			};

			// Add the vertices to draw each line of the bounding box
			for (int i = 0; i < 12; ++i)
			{
				int v1 = edgeIndices[i][0];
				int v2 = edgeIndices[i][1];

				// Add the coordinates of the two vertices for each edge
				lineVerts.push_back(vertices[v1 * 3 + 0]); // x of v1
				lineVerts.push_back(vertices[v1 * 3 + 1]); // y of v1
				lineVerts.push_back(vertices[v1 * 3 + 2]); // z of v1

				lineVerts.push_back(vertices[v2 * 3 + 0]); // x of v2
				lineVerts.push_back(vertices[v2 * 3 + 1]); // y of v2
				lineVerts.push_back(vertices[v2 * 3 + 2]); // z of v2
			}
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
			lineVerts.clear();

			glDeleteVertexArrays(1, &lineVAO);
			glDeleteBuffers(1, &lineVBO);
		}
	}

	// Disable stencil writes
	glStencilMask(0x00);
}

void Renderer::outlinePass(std::vector<Entity*>& outlineRenderList)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	// Disable depth test before drawing outlines
	glDisable(GL_DEPTH_TEST);
	// Use outline shader
	Shader* outlineShader = this->shaderManager.getShader(ShaderType::OUTLINE);
	outlineShader->use();

	for (Entity* outlinedEntity : outlineRenderList)
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

void Renderer::getMeshesRecursively(Frustum& cameraFrustum, std::vector<Entity*>& entities, SortedSceneData& sortedSceneData)
{
	// TODO : Don't unnecessarily sort the scene every frame if there are no updates in between
	// We start by sorting the entities depending on if they are renderable objects
	for (Entity* entity : entities)
	{
		if (entity->getIsEnabled())
		{
			MeshComponent* mesh = entity->getComponent<MeshComponent>();

			if (mesh == nullptr)
				sortedSceneData.logicEntities.push_back(entity);
			else // Entities that can be rendered are grouped by shader
			{
				sortedSceneData.meshes.push_back(mesh);

				if (cameraFrustum.isOnFrustum(mesh->getWorldBoundingBox(), mesh->parent->transform))
				{
					PBRMaterial* pbrMat = dynamic_cast<PBRMaterial*>(mesh->material.get());
					if (pbrMat != nullptr && pbrMat->opacity == 1.0f)
						sortedSceneData.renderList[mesh->material->shaderProgram].push_back(entity);
					else
						sortedSceneData.transparentRenderList[mesh->material->shaderProgram].push_back(entity);

					if (entity->drawOutline)
						sortedSceneData.outlineRenderList.push_back(entity);
				}
				//	mesh->setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
				//else
				//	mesh->setDiffuseColor(glm::vec3(1.0f));
			}

			this->getMeshesRecursively(cameraFrustum, entity->getChildren(), sortedSceneData);
		}
	}
}
