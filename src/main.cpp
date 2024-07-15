#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <utilities/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>

#include "main.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include "shader.hpp"
#include "logger.hpp"
#include "renderTarget.hpp"
#include "materials/pbrMaterial.hpp"
#include "io/interface.hpp"
#include "io/input.hpp"
#include "components/skyboxComponent.hpp"
#include "components/meshComponent.hpp"
#include "components/cameraComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/lights/spotLightComponent.hpp"

extern GLFWwindow* window;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Renderer defaultRenderer;
CameraComponent* cameraComponent;

// Create a new cubemap to store the irradiance when we calculate it
Cubemap* irradianceCubemap = nullptr;
Cubemap* prefilteredMap = nullptr;
Texture* brdfTexture = nullptr;

int main()
{
	if (setupGlfwContext() != 0)
		return -1;
	
	// Sets up some parameters for the OpenGL context
	// Depth test for depth buffering, face culling for performance, blending for transparency
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	irradianceCubemap = new Cubemap(GL_RGB16F, 32, 32);
	PBRMaterial::irradianceMap = irradianceCubemap;

	// Creates a renderer for drawing objects
	defaultRenderer = Renderer();

	Shader* phongShader = defaultRenderer.shaderManager.getShader(ShaderType::PHONG);
	Shader* pbrShader = defaultRenderer.shaderManager.getShader(ShaderType::PBR);
	Shader* gridShader = defaultRenderer.shaderManager.getShader(ShaderType::GRID);
	Shader* skyboxShader = defaultRenderer.shaderManager.getShader(ShaderType::SKYBOX);
	Shader* hdrToCubemapShader = defaultRenderer.shaderManager.getShader(ShaderType::HDRTOCUBEMAP);
	Shader* irradianceShader = defaultRenderer.shaderManager.getShader(ShaderType::IRRADIANCE);
	Shader* prefilterShader = defaultRenderer.shaderManager.getShader(ShaderType::PREFILTER);
	Shader* brdfShader = defaultRenderer.shaderManager.getShader(ShaderType::BRDF);

	LightManager::getInstance().shaderProgram = pbrShader;

	// Directional light
	Entity* dirLightEntity = new Entity("Directional light");
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	defaultRenderer.addEntity(dirLightEntity);

	// Skybox
	Entity* skyEntity = new Entity("Skybox");
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader);
	defaultRenderer.addEntity(skyEntity);

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	for (int i = 0; i < 4; i++)
	{
		// Add point light
		Entity* pointLightEntity = new Entity("Point light");
		pointLightEntity->transform->setScale(glm::vec3(0.1f));
		pointLightEntity->transform->setPosition(lightPositions[i]);
		PointLightComponent* pointLightComponent = pointLightEntity->addComponent<PointLightComponent>();
		pointLightComponent->diffuseColor = lightColors[i];

		defaultRenderer.addEntity(pointLightEntity);
	}

	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	defaultRenderer.init(glm::vec2(windowWidth, windowHeight));
	cameraComponent = defaultRenderer.currentCamera;

	// IBL

	// Add mesh to the light
	float boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };
	Texture* hdrMap = new Texture("img/fouriesburg_mountain_lookout_2_8k.hdr", TextureType::TEXTURE_DIFFUSE, true, true);

	// Add point light
	Entity* cubemapEntity = new Entity("HDR Cubemap");
	MeshComponent* lightMesh = cubemapEntity->addComponent<MeshComponent>();
	lightMesh->setupMesh(boxVertices, sizeof(boxVertices), hdrToCubemapShader);
	lightMesh->addTexture(hdrMap);

	cubemapEntity->start();

	// We create a render target for rendering the HDR map to a cubemap
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	Cubemap* envCubemap = new Cubemap(GL_RGB16F, 2048, 2048);
	envCubemap->bind();

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	hdrToCubemapShader->use()
		->setInt("equirectangularMap", 0)
		->setMat4("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	hdrMap->bindTexture();

	// Convert the 2D map to a cubemap
	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		//defaultRenderer.shaderManager.updateUniformBuffer(captureProjection, captureViews[i]);
		hdrToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set the sky to use the new cubemap
	skyComponent->setCubemap(envCubemap);

	// Resize FBO to cubemap size
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// Create irradiance map
	irradianceShader->use();
	irradianceShader->setInt("environmentMap", 0);
	irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	envCubemap->bind();

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap->texID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightMesh->setupMesh(boxVertices, sizeof(boxVertices), irradianceShader);
		lightMesh->start();
		cubemapEntity->update(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create pre filtered map
	prefilteredMap = new Cubemap(GL_RGB16F, 128, 128, true);
	PBRMaterial::prefilterMap = prefilteredMap;
	
	prefilterShader->use();
	prefilterShader->setInt("environmentMap", 0);
	prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	envCubemap->bind();

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->setFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMap->texID, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lightMesh->setupMesh(boxVertices, sizeof(boxVertices), prefilterShader);
			lightMesh->start();
			cubemapEntity->update(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	skyComponent->setCubemap(envCubemap);

	// Create BRDF map
	float quad_verts[] = {
	-1.0f, 1.0f, 0.0f, // Top left
	1.0f, 1.0f, 0.0f, // Top right
	-1.0f, -1.0f, 0.0f, // Bottom left

	1.0f, 1.0f, 0.0f, // Top right
	1.0f, -1.0f, 0.0f, // Bottom right
	-1.0f, -1.0f, 0.0f, // Bottom left
	};

	float quad_tex_coords[] = {
		0.0f, 0.0f, // Top left
		1.0f, 0.0f, // Top right
		0.0f, 1.0f, // Bottom left

		1.0f, 0.0f, // Top right
		1.0f, 1.0f, // Bottom right
		0.0f, 1.0f, // Bottom left
	};

	Entity* quadEntity = new Entity("Quad");
	MeshComponent* quadMesh = quadEntity->addComponent<MeshComponent>();
	quadMesh->setupMesh(quad_verts, sizeof(quad_verts), brdfShader);
	quadMesh->addTexCoords(quad_tex_coords, sizeof(quad_tex_coords));
	quadMesh->addTexture(hdrMap);

	//defaultRenderer.addEntity(quadEntity);
	quadMesh->start();

	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadEntity->update(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	brdfTexture = new Texture(brdfLUTTexture, TextureType::TEXTURE_ALBEDO);
	PBRMaterial::brdfLut = brdfTexture;

	// Initializes the ImGui UI system
	ImGuiInit(window, &defaultRenderer);
	
	// A simple variable to retrieve the current glGetError() code and decide whether to print it to console
	int glErrorCurrent;
	// A variable that stores the current frame's timestamp, to calculate time between frames
	float currentFrame;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Clears the buffers and last frame before rendering the next one
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculates elapsed time since last frame for time-based calculations
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Processes any mouse or keyboard input for camera movement
		processInput(window, deltaTime);
		
		phongShader->use()
			->setVec3("viewPos", cameraComponent->getPosition());

		pbrShader->use()
			->setVec3("camPos", cameraComponent->getPosition());

		defaultRenderer.render(deltaTime);
		
		// Draws the ImGui interface windows
		ImGuiDrawWindows();

		// Print error code to console if there is one
		glErrorCurrent = glGetError();
		//if (glErrorCurrent != 0) { Logger::logError(std::string("OpenGL error code: ") + std::to_string(glErrorCurrent)); }

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
