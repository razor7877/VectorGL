#include <iostream>
#include <random>

#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>
#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.hpp"
#include "shaderManager.hpp"
#include "entity.hpp"
#include "materials/pbrMaterial.hpp"
#include "components/meshComponent.hpp"
#include "utilities/geometry.hpp"
#include "physics/frustum.hpp"
#include "lightManager.hpp"
#include "logger.hpp"

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    std::string messageSource;
    std::string messageType;
    std::string messageSeverity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        messageSource = "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        messageSource = "WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        messageSource = "SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        messageSource = "THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        messageSource = "APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        messageSource = "UNKNOWN";
        break;

        default:
        messageSource = "UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        messageType = "ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        messageType = "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        messageType = "UNDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        messageType = "PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        messageType = "PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        messageType = "OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        messageType = "MARKER";
        break;

        default:
        messageType = "UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
	        messageSeverity = "HIGH";
	        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
	        messageSeverity = "MEDIUM";
	        break;

        case GL_DEBUG_SEVERITY_LOW:
	        messageSeverity = "LOW";
	        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
	        messageSeverity = "NOTIFICATION";
	        break;

        default:
	        messageSeverity = "UNKNOWN";
	        break;
    }

	std::string logMessage = std::to_string(id) + " - " + messageType + " of " + messageSeverity + ", raised from " + messageSource + ": " + msg;

	if (severity == GL_DEBUG_SEVERITY_HIGH)
		Logger::logError(logMessage, "renderer.cpp");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		Logger::logWarning(logMessage, "renderer.cpp");
	else
		Logger::logInfo(logMessage, "renderer.cpp");

    printf("%d: %s of %s severity, raised from %s: %s\n",
            id, messageType.c_str(), messageSeverity.c_str(), messageSource.c_str(), msg);
}

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

GLuint Renderer::getRenderTexture() const
{
	return this->finalTarget->renderTexture;
}

GLuint Renderer::getSkyRenderTexture() const
{
	return this->skyTarget->renderTexture;
}

void Renderer::resizeFramebuffers(glm::vec2 newSize) const
{
	this->multiSampledTarget->resize(newSize);
	this->finalTarget->resize(newSize);
	this->gBuffer->resize(newSize);
	this->ssaoTarget->resize(newSize * Renderer::SSAO_SCALE_FACTOR);
	this->ssaoBlurTarget->resize(newSize * Renderer::SSAO_SCALE_FACTOR);
	this->ssaoBlurTarget->unbind();
}

void Renderer::init(glm::vec2 lastWindowSize)
{
	// Sets up some parameters for the OpenGL context
	// Depth test for depth buffering
	glEnable(GL_DEPTH_TEST);
	// Stencil test for outlines
	glEnable(GL_STENCIL_TEST);
	// Face culling for performance
	glEnable(GL_CULL_FACE);
	// MSAA
	glEnable(GL_MULTISAMPLE);
	// Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Interpolation between sides of a cubemap
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Enable OpenGL debugging messages with the GL_KHR_debug extension
	// this allows detailed error/warning explanations instead of just the error codes
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLDebugMessageCallback, NULL);

	this->shaderManager.initUniformBuffer();

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

		float scale = static_cast<float>(i) / 64.0f;
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

	this->ssaoNoiseTexture = std::make_unique<Texture>(noiseTexture, TextureType::TEXTURE_2D);

	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::vector<float> quadTexCoords = Geometry::getQuadTexCoords();

	this->ssaoQuad = std::make_unique<Entity>("SSAO_Quad");
	auto* ssaoQuadMesh = ssaoQuad->addComponent<MeshComponent>();
	ssaoQuadMesh->setMaterial(std::make_unique<PBRMaterial>(this->shaderManager.getShader(ShaderType::SSAO)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoQuadMesh->start();

	this->ssaoBlurQuad = std::make_unique<Entity>("SSAO_Blur_Quad");
	auto* ssaoBlurQuadMesh = ssaoBlurQuad->addComponent<MeshComponent>();
	ssaoBlurQuadMesh->setMaterial(std::make_unique<PBRMaterial>(this->shaderManager.getShader(ShaderType::SSAOBLUR)))
		.addVertices(quadVertices)
		.addTexCoords(quadTexCoords);

	ssaoBlurQuadMesh->start();

	this->createFramebuffers(lastWindowSize);
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

void Renderer::render(Scene& scene, PhysicsWorld& physicsWorld, float deltaTime)
{
	double frameStartTime = glfwGetTime();
	double startTime = frameStartTime;

	// Render & update the scene

	// All the entities at the top level of the scene
	std::vector<Entity*> entities = scene.getEntities();
	scene.sortedSceneData.clearCache();
	Frustum frustum(scene.currentCamera, this->multiSampledTarget->size);
	scene.getMeshesRecursively(frustum, entities);

	double endTime = glfwGetTime();
	this->meshSortingTime = endTime - startTime;

	// Update the physics simulation
	startTime = glfwGetTime();
	physicsWorld.update(deltaTime);
	endTime = glfwGetTime();
	this->physicsUpdateTime = endTime - startTime;

	// Render the shadow map
	startTime = glfwGetTime();

	// Update camera info
	glm::vec2 lastWindowSize = this->multiSampledTarget->size;
	this->shaderManager.updateUniformBuffer(scene.currentCamera->getViewMatrix(), scene.currentCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
	this->shaderManager.getShader(ShaderType::PHONG)->use()->setVec3("viewPos", scene.currentCamera->getPosition());
	this->shaderManager.getShader(ShaderType::PBR)->use()->setVec3("camPos", scene.currentCamera->getPosition());
	// Send light data to shader
	LightManager::getInstance().sendToShader();
	this->shadowPass(scene.sortedSceneData.allMeshes, scene);

	endTime = glfwGetTime();
	this->shadowPassTime = endTime - startTime;

	// Render to the G buffer
	startTime = glfwGetTime();
	this->gBufferPass(scene.sortedSceneData.meshes);
	endTime = glfwGetTime();
	this->gBufferPassTime = endTime - startTime;

	// Calculate SSAO
	startTime = glfwGetTime();
	this->ssaoPass(scene.sortedSceneData.meshes);
	endTime = glfwGetTime();
	this->ssaoPassTime = endTime - startTime;

	// Render the scene
	startTime = glfwGetTime();

	// We now want to draw to the MSAA framebuffer
	this->multiSampledTarget->bind();
	this->multiSampledTarget->clear();
	this->renderPass(deltaTime, physicsWorld, scene.sortedSceneData);

	endTime = glfwGetTime();
	this->renderPassTime = endTime - startTime;

	// Render outlines
	startTime = glfwGetTime();
	this->outlinePass(scene.sortedSceneData.outlineRenderList);
	this->multiSampledTarget->unbind();
	endTime = glfwGetTime();
	this->outlinePassTime = endTime - startTime;

	// Resolve the multisampled framebuffer to the normal one for display
	startTime = glfwGetTime();
	this->blitPass();
	endTime = glfwGetTime();
	this->blitPassTime = endTime - startTime;

	startTime = glfwGetTime();

	if (this->enableDebugDraw && scene.skyCamera != nullptr)
	{
		this->skyTarget->bind();
		this->skyTarget->clear();

		this->shaderManager.updateUniformBuffer(scene.skyCamera->getViewMatrix(), scene.skyCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
		this->renderPass(deltaTime, physicsWorld, scene.sortedSceneData);

		this->skyTarget->unbind();
	}

	endTime = glfwGetTime();
	this->debugPassTime = endTime - startTime;

	this->frameRenderTime = glfwGetTime() - frameStartTime;
}

void Renderer::end()
{
	this->multiSampledTarget.release();
	this->finalTarget.release();
	this->skyTarget.release();
	this->depthMap.release();
	this->gBuffer.release();
	this->ssaoTarget.release();
	this->ssaoBlurTarget.release();
	this->ssaoNoiseTexture.release();
	this->ssaoQuad.release();
	this->ssaoBlurQuad.release();

	this->shaderManager.end();
}

void Renderer::createFramebuffers(glm::vec2 lastWindowSize)
{
	this->multiSampledTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D_MULTISAMPLE, lastWindowSize);
	this->finalTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, lastWindowSize);
	this->skyTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, lastWindowSize);

	// Shadow mapping
	this->depthMap = std::make_unique<RenderTarget>(TargetType::TEXTURE_DEPTH_3D, glm::vec2(Renderer::SHADOW_MAP_WIDTH, Renderer::SHADOW_MAP_HEIGHT), GL_DEPTH_COMPONENT);
	PBRMaterial::shadowMap = std::make_shared<TextureView>(this->depthMap->renderTexture, TextureType::TEXTURE_3D);

	// Screen space effects
	this->gBuffer = std::make_unique<RenderTarget>(TargetType::G_BUFFER, lastWindowSize, GL_RGBA16F);
	this->ssaoTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, lastWindowSize * Renderer::SSAO_SCALE_FACTOR, GL_RED);
	this->ssaoBlurTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, lastWindowSize * Renderer::SSAO_SCALE_FACTOR, GL_RED);
	PBRMaterial::ssaoMap = std::make_unique<TextureView>(this->ssaoBlurTarget->renderTexture, TextureType::TEXTURE_2D);
}

glm::mat4 Renderer::getLightSpaceMatrix(const Scene& scene, const float nearPlane, const float farPlane) const
{
	glm::mat4 cameraProjection = glm::perspective(
		glm::radians(scene.currentCamera->getZoom()),
		(float)this->multiSampledTarget->size.x / (float)this->multiSampledTarget->size.y,
		nearPlane,
		farPlane
	);
	glm::mat4 cameraView = scene.currentCamera->getViewMatrix();

	// Get the corners of the camera frustum
	std::vector<glm::vec4> frustumCorners = Frustum::getFrustumCornersWorldSpace(cameraProjection, cameraView);

	auto frustumCenter = glm::vec3(0.0f);

	for (const auto& v : frustumCorners)
		frustumCenter += glm::vec3(v);

	// Average the positions to get the frustum center
	frustumCenter /= frustumCorners.size();

	glm::vec3 lightDir = glm::normalize(scene.directionalLight->parent->getTransform()->getPosition());
	const glm::mat4 lightView = glm::lookAt(
		frustumCenter + lightDir,
		frustumCenter,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (const auto& v : frustumCorners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	constexpr float zMult = 10.0f;

	if (minZ < 0)
		minZ *= zMult;
	else
		minZ /= zMult;

	if (maxZ < 0)
		maxZ /= zMult;
	else
		maxZ *= zMult;

	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

void Renderer::shadowPass(const std::vector<MeshComponent*>& meshes, const Scene& scene)
{
	float near = CameraComponent::NEAR;
	float far = CameraComponent::FAR;
	float cascadeLevels[3] = {
		far * Renderer::SHADOW_CASCADE_DISTANCES[0],
		far * Renderer::SHADOW_CASCADE_DISTANCES[1],
		far * Renderer::SHADOW_CASCADE_DISTANCES[2]
	};

	std::vector<glm::mat4> lightSpaceMatrices = {
		this->getLightSpaceMatrix(scene, near, cascadeLevels[0]),
		this->getLightSpaceMatrix(scene, cascadeLevels[0], cascadeLevels[1]),
		this->getLightSpaceMatrix(scene, cascadeLevels[1], cascadeLevels[2]),
		this->getLightSpaceMatrix(scene, cascadeLevels[2], far)
	};

	for (int i = 0; i < 4; i++)
		PBRMaterial::lightSpaceMatrices[i] = lightSpaceMatrices[i];

	PBRMaterial::cascadePlaneDistances[0] = cascadeLevels[0];
	PBRMaterial::cascadePlaneDistances[1] = cascadeLevels[1];
	PBRMaterial::cascadePlaneDistances[2] = cascadeLevels[2];

	PBRMaterial::farPlane = far;

	Shader* depthShader = this->shaderManager.getShader(ShaderType::DEPTH_CASCADED);

	depthShader->use()
		->setMat4("lightSpaceMatrices[0]", lightSpaceMatrices[0])
		->setMat4("lightSpaceMatrices[1]", lightSpaceMatrices[1])
		->setMat4("lightSpaceMatrices[2]", lightSpaceMatrices[2])
		->setMat4("lightSpaceMatrices[3]", lightSpaceMatrices[3]);

	this->depthMap->bind();
	this->depthMap->clear();

	glEnable(GL_DEPTH_TEST);

	for (MeshComponent* mesh : meshes)
		mesh->drawGeometry(depthShader);

	this->depthMap->unbind();
}

void Renderer::gBufferPass(const std::vector<MeshComponent*>& meshes)
{
	this->gBuffer->bind();
	this->gBuffer->clear();

	// Use G-buffer shader
	Shader* gBufferShader = this->shaderManager.getShader(ShaderType::GBUFFER);
	gBufferShader->use();

	for (MeshComponent* mesh : meshes)
		mesh->drawGeometry(gBufferShader);

	this->gBuffer->unbind();
}

void Renderer::ssaoPass(std::vector<MeshComponent*>& meshes)
{
	this->ssaoTarget->bind();
	this->ssaoTarget->clear();

	Shader* ssaoShader = this->shaderManager.getShader(ShaderType::SSAO);
	// Setup required uniforms
	ssaoShader->use()
		->setInt("gPosition", 0)
		->setInt("gNormal", 1)
		->setInt("texNoise", 2)
		->setVec2("noiseScale", glm::vec2(this->ssaoTarget->size.x / 4.0f, this->ssaoTarget->size.y / 4.0f));

	for (unsigned int i = 0; i < 64; i++)
		ssaoShader->setVec3("samples[" + std::to_string(i) + "]", this->ssaoKernel[i]);

	// Bind the G buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->gBuffer->gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->gBuffer->gNormal);

	glActiveTexture(GL_TEXTURE2);
	this->ssaoNoiseTexture->bindTexture();

	// Render SSAO to quad
	this->ssaoQuad->getComponent<MeshComponent>()->drawGeometry(ssaoShader);

	this->ssaoTarget->unbind();

	// Now we want to blur the result to correct the repeating noise pattern
	this->ssaoBlurTarget->bind();
	this->ssaoBlurTarget->clear();

	Shader* ssaoBlurShader = this->shaderManager.getShader(ShaderType::SSAOBLUR);
	ssaoBlurShader->use()
		->setInt("ssaoInput", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->ssaoTarget->renderTexture);

	this->ssaoBlurQuad->getComponent<MeshComponent>()->drawGeometry(ssaoBlurShader);

	this->ssaoBlurTarget->unbind();
}

void Renderer::renderPass(float deltaTime, PhysicsWorld& physicsWorld, SortedSceneData& sceneData)
{
	glStencilMask(0x00);

	this->shaderManager.getShader(ShaderType::PBR)
		->use()
		->setVec2("windowSize", this->ssaoTarget->size);

	// We can simply update all entities that won't be rendered
	for (Entity* nonRenderable : sceneData.logicEntities)
		nonRenderable->update(deltaTime);

	for (PhysicsComponent* physics : sceneData.physicsComponents)
		physics->update(deltaTime);

	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	// Entities that can be rendered are grouped by shader and then rendered together
	for (auto& [shader, meshes] : sceneData.renderList)
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

	for (auto& [shader, meshesByDistance] : sceneData.transparentRenderList)
	{
		shader->use();

		for (auto it = meshesByDistance.rbegin(); it != meshesByDistance.rend(); ++it)
		{
			// We only write to the stencil mask if the entity should have an outline
			if (it->second->drawOutline)
				glStencilMask(0xFF);
			else
				glStencilMask(0x00);

			it->second->update(deltaTime);
		}
	}

	if (this->enableDebugDraw)
	{
		// Line drawing for debugging raycasts etc.
		std::vector<float> debugLines = physicsWorld.getDebugLines();
		lineVerts.insert(lineVerts.end(), debugLines.begin(), debugLines.end());
		lineVerts.insert(lineVerts.end(), storedLineVerts.begin(), storedLineVerts.end());

		// Debug bounding boxes
		for (MeshComponent* mesh : sceneData.meshes)
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

			// Add the vertices to draw each line of the bounding box
			for (int i = 0; i < 12; ++i)
			{
				int edgeIndices[12][2] = {
					{0, 1}, {0, 2}, {1, 3}, {2, 3}, // Left side edges
					{4, 5}, {4, 6}, {5, 7}, {6, 7}, // Right side edges
					{0, 4}, {2, 6}, {1, 5}, {3, 7}, // Connect the two sides
				};

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

		if (!lineVerts.empty())
		{
			GLuint lineVAO;
			GLuint lineVBO;

			glGenVertexArrays(1, &lineVAO);
			glGenBuffers(1, &lineVBO);
			glBindVertexArray(lineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
			glBufferData(GL_ARRAY_BUFFER, lineVerts.size() * sizeof(float), &lineVerts[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
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

void Renderer::outlinePass(const std::vector<Entity*>& outlineRenderList)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	// Disable depth test before drawing outlines
	glDisable(GL_DEPTH_TEST);
	// Use outline shader
	Shader* outlineShader = this->shaderManager.getShader(ShaderType::OUTLINE);
	outlineShader->use();

	for (Entity* outlinedEntity : outlineRenderList)
	{
		auto* mesh = outlinedEntity->getComponent<MeshComponent>();
		glm::vec3 originalScale = outlinedEntity->getTransform()->getScale();
		Shader* originalShader = mesh->material->shaderProgram;

		mesh->material->shaderProgram = outlineShader;

		outlinedEntity->getTransform()->setScale(originalScale * 1.1f);
		outlinedEntity->update(0);
		outlinedEntity->getTransform()->setScale(originalScale);

		mesh->material->shaderProgram = originalShader;
	}

	// Reenable depth test after drawing outlines
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
	// Reenable stencil writes or buffer won't be cleared properly on next frame
	glStencilMask(0xFF);
}

void Renderer::blitPass() const
{
	// Bind the second target that will contain the mixed multisampled textures
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->multiSampledTarget->framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->finalTarget->framebuffer);

	glm::vec2 framebufferSize = this->multiSampledTarget->size;
	// Resolve the multisampled texture to the second target
	glScissor(0, 0, framebufferSize.x, framebufferSize.y);
	glBlitFramebuffer(0, 0, framebufferSize.x, framebufferSize.y, 0, 0, framebufferSize.x, framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
