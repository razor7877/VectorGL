#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utilities/stb_image.h>

#include "main.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "cubemap.hpp"
#include "renderer.hpp"
#include "materials/material.hpp"
#include "logger.hpp"

#include "io/interface.hpp"
#include "io/input.hpp"

#include "components/meshComponent.hpp"
#include "components/transformComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/spotLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/skyboxComponent.hpp"

#include "utilities/resourceLoader.hpp"

extern GLFWwindow* window;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Renderer defaultRenderer;

CameraComponent* cameraComponent;

int main()
{
	if (setupGlfwContext() != 0)
		return -1;
	
	// Sets up some parameters for the OpenGL context
	// Depth test for depth buffering, face culling for performance, blending for transparency
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Creates a renderer for drawing objects
	defaultRenderer = Renderer();

	Shader* phongShader = defaultRenderer.shaderManager.getShader(ShaderType::PHONG);
	Shader* pbrShader = defaultRenderer.shaderManager.getShader(ShaderType::PBR);
	Shader* gridShader = defaultRenderer.shaderManager.getShader(ShaderType::GRID);
	Shader* skyboxShader = defaultRenderer.shaderManager.getShader(ShaderType::SKYBOX);

	LightManager::getInstance().shaderProgram = pbrShader;

	float gridVerts[18] = { 1, 1, 0, -1, -1, 0, -1, 1, 0,
		-1, -1, 0, 1, 1, 0, 1, -1, 0 };
	Entity* grid = new Entity("Grid");
	MeshComponent* gridMesh = grid->addComponent<MeshComponent>();
	gridMesh->setupMesh(gridVerts, sizeof(gridVerts), gridShader);

	defaultRenderer.addEntity(grid);
	
	/*Entity* model = ResourceLoader::getInstance().loadModelFromFilepath("models/sea_keep/scene.gltf", pbrShader);
	model->transform
		->setScale(0.075f, 0.075f, 0.075f);

	defaultRenderer.addEntity(model);*/

	// Add point light
	Entity* pointLightEntity = new Entity("Point light");
	pointLightEntity->transform->setScale(glm::vec3(0.1f));
	pointLightEntity->transform->setPosition(0.0f, 5.0f, 0.0f);
	PointLightComponent* pointLightComponent = pointLightEntity->addComponent<PointLightComponent>();

	// Add mesh to the light
	float boxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

	MeshComponent* lightMesh = pointLightEntity->addComponent<MeshComponent>();
	lightMesh->setupMesh(boxVertices, sizeof(boxVertices), pbrShader);

	defaultRenderer.addEntity(pointLightEntity);

	// Add spot light
	Entity* spotLightEntity = new Entity("Spot light");
	SpotLightComponent* spotLightComponent = spotLightEntity->addComponent<SpotLightComponent>();

	defaultRenderer.addEntity(spotLightEntity);

	Entity* dirLightEntity = new Entity("Directional light");
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();

	defaultRenderer.addEntity(dirLightEntity);

	Entity* skyEntity = new Entity("Skybox");
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader);

	defaultRenderer.addEntity(skyEntity);

	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	defaultRenderer.init(glm::vec2(windowWidth, windowHeight));
	cameraComponent = defaultRenderer.currentCamera;

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
