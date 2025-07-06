#include <iostream>

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

	this->gBufferPass->renderTarget->resize(newSize);
	this->ssaoPass->ssaoTarget->resize(newSize * SSAOPass::SSAO_SCALE_FACTOR);
	this->ssaoPass->renderTarget->resize(newSize * SSAOPass::SSAO_SCALE_FACTOR);
	PBRMaterial::ssaoMap = std::make_unique<TextureView>(this->ssaoPass->renderTarget->renderTexture, TextureType::TEXTURE_2D);
	this->ssaoPass->renderTarget->unbind();
}

glm::vec2 Renderer::getRenderSize() const
{
	return this->multiSampledTarget->size;
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

	this->shadowPass = std::make_unique<ShadowPass>();
	this->gBufferPass = std::make_unique<GBufferPass>();
	this->ssaoPass = std::make_unique<SSAOPass>(*this, *gBufferPass.get());

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
	// Helper lambda to measure the execution time of the different render sections
	auto measureTime = [](double& outTime, const std::function<void()>& func) {
		double start = glfwGetTime();
		func();
		// This makes sure the GPU finishes running all the calculations for the pass before measuring time
		glFinish();
		outTime = glfwGetTime() - start;
	};

	double frameStartTime = glfwGetTime();

	// Render & update the scene

	measureTime(this->meshSortingTime, [&]() {
		// All the entities at the top level of the scene
		std::vector<Entity*> entities = scene.getEntities();
		scene.sortedSceneData.clearCache();
		Frustum frustum(scene.currentCamera, this->multiSampledTarget->size);
		scene.getMeshesRecursively(frustum, entities);
	});

	measureTime(this->physicsUpdateTime, [&]() {
		// Update the physics simulation
		physicsWorld.update(deltaTime);
	});

	glm::vec2 lastWindowSize = this->multiSampledTarget->size;

	// Update camera info
	this->shaderManager.updateUniformBuffer(scene.currentCamera->getViewMatrix(), scene.currentCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
	this->shaderManager.getShader(ShaderType::PHONG)->use()->setVec3("viewPos", scene.currentCamera->getPosition());
	this->shaderManager.getShader(ShaderType::PBR)->use()->setVec3("camPos", scene.currentCamera->getPosition());
	// Send light data to shader
	LightManager::getInstance().sendToShader();

	measureTime(this->shadowPassTime, [&]() {
		// Render the shadow map
		this->shadowPass->execute(*this, scene, deltaTime);
	});

	measureTime(this->gBufferPassTime, [&]() {
		// Render to the G buffer
		this->gBufferPass->execute(*this, scene, deltaTime);
	});

	measureTime(this->ssaoPassTime, [&]() {
		// Calculate SSAO
		this->ssaoPass->execute(*this, scene, deltaTime);
	});

	// We now want to draw to the MSAA framebuffer
	this->multiSampledTarget->bind();
	this->multiSampledTarget->clear();

	measureTime(this->renderPassTime, [&]() {
		// Render the scene
		this->renderPass(deltaTime, physicsWorld, scene.sortedSceneData);
	});

	measureTime(this->outlinePassTime, [&]() {
		// Render outlines
		this->outlinePass(scene.sortedSceneData.outlineRenderList);
	});

	this->multiSampledTarget->unbind();

	measureTime(this->blitPassTime, [&]() {
		// Resolve the multisampled framebuffer to the normal one for display
		this->blitPass();
	});

	measureTime(this->debugPassTime, [&]() {
		if (this->enableDebugDraw && scene.skyCamera != nullptr)
		{
			this->skyTarget->bind();
			this->skyTarget->clear();

			this->shaderManager.updateUniformBuffer(scene.skyCamera->getViewMatrix(), scene.skyCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
			this->renderPass(deltaTime, physicsWorld, scene.sortedSceneData);

			this->skyTarget->unbind();
		}
	});

	this->frameRenderTime = glfwGetTime() - frameStartTime;
}

void Renderer::end()
{
	this->multiSampledTarget.release();
	this->finalTarget.release();
	this->skyTarget.release();
	this->depthMap.release();

	this->shadowPass.release();
	this->gBufferPass.release();
	this->ssaoPass.release();

	this->shaderManager.end();
}

void Renderer::createFramebuffers(glm::vec2 lastWindowSize)
{
	this->multiSampledTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D_MULTISAMPLE, lastWindowSize);
	this->finalTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, lastWindowSize);
	this->skyTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, lastWindowSize);

	// Screen space effects
	this->gBufferPass->renderTarget = std::make_unique<RenderTarget>(TargetType::G_BUFFER, lastWindowSize, GL_RGBA16F);
	this->ssaoPass->ssaoTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, lastWindowSize * SSAOPass::SSAO_SCALE_FACTOR, GL_RED);
	this->ssaoPass->renderTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, lastWindowSize * SSAOPass::SSAO_SCALE_FACTOR, GL_RED);
	PBRMaterial::ssaoMap = std::make_unique<TextureView>(this->ssaoPass->renderTarget->renderTexture, TextureType::TEXTURE_2D);
}

void Renderer::renderPass(float deltaTime, PhysicsWorld& physicsWorld, SortedSceneData& sceneData)
{
	glStencilMask(0x00);

	this->shaderManager.getShader(ShaderType::PBR)
		->use()
		->setVec2("windowSize", this->ssaoPass->renderTarget->size);

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
		// TODO: Fix shadow bug. Whenever an object is outlined, the shadow on the next frame end up bigger even though the object was scaled back down, maybe depth or stencil issue?
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
