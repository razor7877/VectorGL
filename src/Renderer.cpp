#include <iostream>

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "ShaderManager.hpp"
#include "Entity.hpp"
#include "materials/PBRMaterial.hpp"
#include "utilities/geometry.hpp"
#include "physics/Frustum.hpp"
#include "LightManager.hpp"
#include "Logger.hpp"
#include "utilities/Timing.hpp"

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
	this->finalTarget->resize(newSize);

	this->gBufferPass->renderTarget->resize(newSize);
	this->ssaoPass->ssaoTarget->resize(newSize * SSAOPass::SSAO_SCALE_FACTOR);
	this->ssaoPass->renderTarget->resize(newSize * SSAOPass::SSAO_SCALE_FACTOR);
	PBRMaterial::ssaoMap = std::make_unique<TextureView>(this->ssaoPass->renderTarget->renderTexture, TextureType::TEXTURE_2D);
	this->ssaoPass->renderTarget->unbind();
	 this->reflectionPass->renderTarget->resize(newSize);
	this->mainRenderPass->renderTarget->resize(newSize);
}

glm::vec2 Renderer::getRenderSize() const
{
	return this->mainRenderPass->renderTarget->size;
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

	this->staticShadowPass = std::make_unique<ShadowPass>();
	this->gBufferPass = std::make_unique<GBufferPass>();
	this->ssaoPass = std::make_unique<SSAOPass>(*this, *gBufferPass);
	this->reflectionPass = std::make_unique<ReflectionPass>(*this, *gBufferPass);
	this->mainRenderPass = std::make_unique<MainRenderPass>();
	this->debugRenderPass = std::make_unique<DebugRenderPass>();
	this->outlinePass = std::make_unique<OutlinePass>();
	this->blitPass = std::make_unique<BlitPass>();

	this->createRenderTargets(lastWindowSize);
}

void Renderer::render(Scene& scene, const PhysicsWorld& physicsWorld, float deltaTime)
{
	double frameStartTime = glfwGetTime();
	glm::vec2 lastWindowSize = this->mainRenderPass->renderTarget->size;

	// Render & update the scene

	this->meshSortingTime = Timing::measureTime([&]() {
		// All the entities at the top level of the scene
		std::vector<Entity*> entities = scene.getEntities();
		scene.sortedSceneData.clearCache();
		Frustum frustum(scene.currentCamera, lastWindowSize);
		scene.getMeshesRecursively(frustum, entities);
	});

	this->physicsUpdateTime = Timing::measureTime([&]() {
		// Update the physics simulation
		physicsWorld.update(deltaTime);
	});

	// Update camera info
	this->shaderManager.updateUniformBuffer(scene.currentCamera->getViewMatrix(), scene.currentCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
	this->shaderManager.getShader(ShaderType::PHONG)->use()->setVec3("viewPos", scene.currentCamera->getPosition());
	this->shaderManager.getShader(ShaderType::PBR)->use()->setVec3("camPos", scene.currentCamera->getPosition());
	// Send light data to shader
	LightManager::getInstance().sendToShader();

	this->shadowPassTime = Timing::measureTime([&]() {
		// Render the shadow map
		this->staticShadowPass->execute(*this, scene, deltaTime);
	});

	this->gBufferPassTime = Timing::measureTime([&]() {
		// Render to the G buffer
		this->gBufferPass->execute(*this, scene, deltaTime);
	});

	this->ssaoPassTime = Timing::measureTime([&]() {
		// Calculate SSAO
		this->ssaoPass->execute(*this, scene, deltaTime);
	});

	this->reflectionPass->execute(*this, scene, deltaTime);

	this->renderPassTime = Timing::measureTime([&]() {
		// Render the scene
		this->mainRenderPass->execute(*this, scene, deltaTime);
	});

	if (this->enableDebugDraw)
	{
		// Line drawing for debugging raycasts etc.
		std::vector<float> physicsLines = physicsWorld.getDebugLines();
		this->debugRenderPass->addLines(physicsLines, false);
		this->debugRenderPass->execute(*this->mainRenderPass->renderTarget, *this, scene, deltaTime);
	}

	this->outlinePassTime = Timing::measureTime([&]() {
		// Render outlines
		this->outlinePass->execute(*this->mainRenderPass->renderTarget, *this, scene, deltaTime);
	});

	this->blitPassTime = Timing::measureTime([&]() {
		// Resolve the multi sampled framebuffer to the normal one for display
		this->blitPass->execute(*this->mainRenderPass->renderTarget, *this->finalTarget, *this, scene, deltaTime);
		//this->blitPass->execute(*this->reflectionPass->renderTarget, *this->finalTarget, *this, scene, deltaTime);
	});

	this->debugPassTime = Timing::measureTime([&]() {
		if (this->enableDebugDraw && scene.skyCamera != nullptr)
		{
			this->skyTarget->bind();
			this->skyTarget->clear();

			this->shaderManager.updateUniformBuffer(scene.skyCamera->getViewMatrix(), scene.skyCamera->getProjectionMatrix(lastWindowSize.x, lastWindowSize.y));
			this->mainRenderPass->execute(*this, scene, deltaTime);

			this->skyTarget->unbind();
		}
	});

	this->frameRenderTime = glfwGetTime() - frameStartTime;
}

void Renderer::end()
{
	this->finalTarget.reset();
	this->skyTarget.reset();
	this->depthMap.reset();

	this->staticShadowPass.reset();
	this->gBufferPass.reset();
	this->ssaoPass.reset();
	this->reflectionPass.reset();
	this->mainRenderPass.reset();
	this->debugRenderPass.reset();
	this->outlinePass.reset();
	this->blitPass.reset();

	this->shaderManager.end();
}

void Renderer::createRenderTargets(glm::vec2 windowSize)
{
	// Screen space effects
	this->gBufferPass->renderTarget = std::make_unique<RenderTarget>(TargetType::G_BUFFER, windowSize, GL_RGBA16F);
	this->ssaoPass->ssaoTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, windowSize * SSAOPass::SSAO_SCALE_FACTOR, GL_RED);
	this->ssaoPass->renderTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_RED, windowSize * SSAOPass::SSAO_SCALE_FACTOR, GL_RED);
	PBRMaterial::ssaoMap = std::make_unique<TextureView>(this->ssaoPass->renderTarget->renderTexture, TextureType::TEXTURE_2D);
	this->reflectionPass->renderTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, windowSize);

	this->mainRenderPass->renderTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D_MULTISAMPLE, windowSize);

	this->finalTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, windowSize);
	this->skyTarget = std::make_unique<RenderTarget>(TargetType::TEXTURE_2D, windowSize);
}

