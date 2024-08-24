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

	// Entities that are rendered to the screen
	std::map<MaterialType, std::vector<Entity*>> renderables;
	// Entities that should have an outline
	std::vector<Entity*> outlineRenderables;
	std::vector<MeshComponent*> meshes;
	// Entities that aren't rendered to the screen
	std::vector<Entity*> nonRenderables;
	std::map<MaterialType, std::vector<Entity*>> transparentRenderables;

	std::vector<Entity*> ptrList;
	for (auto&& entity : this->entities)
		ptrList.push_back(entity.get());

	double startTime = glfwGetTime();
	this->getMeshesRecursively(ptrList, renderables, outlineRenderables, meshes, nonRenderables, transparentRenderables);
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
	this->shadowPass(meshes);
	endTime = glfwGetTime();
	this->shadowPassTime = endTime - startTime;

	// Render to the G buffer
	startTime = glfwGetTime();
	this->gBufferPass(meshes);
	endTime = glfwGetTime();
	this->gBufferPassTime = endTime - startTime;

	// Calculate SSAO
	startTime = glfwGetTime();
	this->ssaoPass(meshes);
	endTime = glfwGetTime();
	this->ssaoPassTime = endTime - startTime;
	
	// We now want to draw to the MSAA framebuffer
	this->multiSampledTarget.bind();
	this->multiSampledTarget.clear();

	// Render the scene
	startTime = glfwGetTime();
	this->renderPass(deltaTime, renderables, nonRenderables, transparentRenderables);
	endTime = glfwGetTime();
	this->renderPassTime = endTime - startTime;

	// Render outlines
	startTime = glfwGetTime();
	this->outlinePass(outlineRenderables);
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

	this->shaderManager.updateUniformBuffer(this->skyCamera->getViewMatrix(), this->skyCamera->getProjectionMatrix(windowSize.x, windowSize.y));
	this->renderPass(deltaTime, renderables, nonRenderables, transparentRenderables);

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
		mesh->update(0);
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
		mesh->update(0);
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

void Renderer::renderPass(float deltaTime, std::map<MaterialType, std::vector<Entity*>>& renderables, std::vector<Entity*>& nonRenderables, std::map<MaterialType, std::vector<Entity*>>& transparentRenderables)
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

	for (auto& [material, meshes] : transparentRenderables)
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

float getSignedDistanceToPlane(const Plane& plane, const glm::vec3& point)
{
	return glm::dot(plane.normal, point - plane.position) - plane.distance;
}

bool isOnOrForwardPlane(const Plane& plane, glm::vec3 extents, glm::vec3 center)
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = extents.x * std::abs(plane.normal.x) +
		extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

	return -r <= getSignedDistanceToPlane(plane, center);
}

bool isOnFrustum(const Frustum& camFrustum, TransformComponent* transform, BoundingBox meshBB)
{
	glm::vec3 bbCenter = (meshBB.maxPosition + meshBB.minPosition) * 0.5f;
	glm::vec3 bbExtents = meshBB.maxPosition - bbCenter;

	glm::vec3 transformRotation = transform->getRotation();
	glm::vec3 transformUp = transformRotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 transformRight = transformRotation * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 transformForward = transformRotation * glm::vec3(1.0f, 0.0f, 0.0f);

	glm::mat4 globalModelMatrix = transform->getGlobalModelMatrix();
	glm::vec3 globalCenter(globalModelMatrix[3][0], globalModelMatrix[3][1], globalModelMatrix[3][2]);

	glm::vec3 right = transformRight * bbExtents.x;
	glm::vec3 up = transformUp * bbExtents.y;
	glm::vec3 forward = transformForward * bbExtents.z;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	glm::vec3 newExtents(newIi, newIj, newIk);

	return (isOnOrForwardPlane(camFrustum.leftFace, newExtents, globalCenter) &&
		isOnOrForwardPlane(camFrustum.rightFace, newExtents, globalCenter) &&
		isOnOrForwardPlane(camFrustum.topFace, newExtents, globalCenter) &&
		isOnOrForwardPlane(camFrustum.bottomFace, newExtents, globalCenter) &&
		isOnOrForwardPlane(camFrustum.nearFace, newExtents, globalCenter) &&
		isOnOrForwardPlane(camFrustum.farFace, newExtents, globalCenter));
}

void Renderer::getMeshesRecursively(
	std::vector<Entity*> entities,
	std::map<MaterialType, std::vector<Entity*>>& renderables,
	std::vector<Entity*>& outlineRenderables,
	std::vector<MeshComponent*>& meshes,
	std::vector<Entity*>& nonRenderables,
	std::map<MaterialType, std::vector<Entity*>>& transparentRenderables)
{
	Frustum frustum;

	const float halfVSide = this->currentCamera->FAR * tanf(this->currentCamera->getZoom() * 0.5f);
	const float halfHSide = halfVSide * (this->multiSampledTarget.size.x / this->multiSampledTarget.size.y);

	const glm::vec3 frontMultFar = this->currentCamera->FAR * this->currentCamera->getForward();

	glm::vec3 camPos = this->currentCamera->getPosition();
	glm::vec3 camForward = this->currentCamera->getForward();
	//glm::vec3 camRight = this->currentCamera->getRight();
	//glm::vec3 camUp = this->currentCamera->getUp();
	glm::vec3 camRight = glm::cross(this->currentCamera->getForward(), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 camUp = glm::cross(camRight, this->currentCamera->getForward());

	frustum.nearFace = { camPos + this->currentCamera->NEAR * camForward, camForward };
	frustum.farFace = { camPos + frontMultFar, -camForward };
	frustum.rightFace = { camPos, glm::cross(frontMultFar - camRight * halfHSide, camUp) };
	frustum.leftFace = { camPos, glm::cross(camUp, frontMultFar + camRight * halfHSide) };
	frustum.topFace = { camPos, glm::cross(camRight, frontMultFar - camUp * halfVSide) };
	frustum.bottomFace = { camPos, glm::cross(frontMultFar + camUp * halfVSide, camRight) };

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
				BoundingBox meshBB = mesh->getBoundingBox();
				if (isOnFrustum(frustum, entity->transform, meshBB))
					mesh->setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
				else
					mesh->setDiffuseColor(glm::vec3(1.0f));
				
				meshes.push_back(mesh);

				PBRMaterial* pbrMat = dynamic_cast<PBRMaterial*>(mesh->material.get());
				if (pbrMat != nullptr && pbrMat->opacity == 1.0f)
					renderables[mesh->material->getType()].push_back(entity);
				else
					transparentRenderables[mesh->material->getType()].push_back(entity);
				if (entity->drawOutline)
					outlineRenderables.push_back(entity);
			}

			this->getMeshesRecursively(entity->getChildren(), renderables, outlineRenderables, meshes, nonRenderables, transparentRenderables);
		}
	}
}
