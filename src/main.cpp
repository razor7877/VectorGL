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
#include <stb_image.h>

#include "main.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "Mesh.hpp"
#include "skybox.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "io/interface.hpp"
#include "io/input.hpp"
#include "cubemap.hpp"
#include "renderer.hpp"
#include "lights/directionalLight.hpp"
#include "lights/pointLight.hpp"
#include "lights/spotLight.hpp"



const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));

// Uniform buffer object (global uniforms)
unsigned int UBO;
Renderer defaultRenderer;

// Sets up necessary data for the uniform buffer
void initUniformBuffer()
{
	glGenBuffers(1, &UBO);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Make sure buffer range corresponds to actual buffer size!
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));
}

// Updates the uniform buffer's data
void updateUniformBuffer(glm::mat4 view, glm::mat4 projection)
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &view[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &projection[0][0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int main()
{
	if (setupGlfwContext() != 0)
	{
		return -1;
	}
	
	// Sets up some parameters for the OpenGL context
	// Depth test for depth buffering, face culling for performance, blending for transparency
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* phongShader = new Shader("shaders/phong.vert", "shaders/phong.frag");
	Shader* skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
	Shader* gridShader = new Shader("shaders/grid2.vert", "shaders/grid2.frag");

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);

	// Sets up global uniforms for each shader used
	initUniformBuffer();

	unsigned int UBIphongShader = glGetUniformBlockIndex(phongShader->ID, "Matrices");
	glUniformBlockBinding(phongShader->ID, UBIphongShader, 0);

	unsigned int UBIskyboxShader = glGetUniformBlockIndex(skyboxShader->ID, "Matrices");
	glUniformBlockBinding(skyboxShader->ID, UBIskyboxShader, 0);

	updateUniformBuffer(view, projection);
	
	Texture* crate = new Texture("img/container2.png", "texture_diffuse", false);
	Material* boxTex = new Material(crate);

	glm::vec3 ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	float shininess = 32.0f;

	// Sets up variables for the phong lighting shader
	phongShader->use()
		->setInt("texture1", 0)
		->setVec3("viewPos", camera.position)
		->setVec3("material.ambient", boxTex->ambient)
		->setVec3("material.diffuse", boxTex->diffuse)
		->setVec3("material.specular", boxTex->specular)
		->setFloat("material.shininess", boxTex->shininess);

	// Creates a renderer for drawing objects
	defaultRenderer = Renderer(phongShader->ID);

	// Sets up lighting for the renderer's LightManager
	PointLight pointLight = PointLight(glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(15.0f, 5.0f, 5.0f), 1.0f, 0.045f, 0.0075f);
	PointLight pointLight2 = PointLight(glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-5.0f, 5.0f, -5.0f), 1.0f, 0.045f, 0.0075f);
	DirectionalLight dirLight = DirectionalLight(glm::vec3(0.4f), glm::vec3(0.7f), glm::vec3(1.0f), glm::vec3(-0.2f, -1.0f, -0.3f));
	SpotLight spotLight = SpotLight(glm::vec3(0.6f), glm::vec3(0.8f), glm::vec3(1.0f), camera.position, 1.0f, 0.09f, 0.032f, camera.front, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
	SpotLight sl2 = SpotLight(glm::vec3(0.0f, 0.0f, 0.6f), glm::vec3(0.0f, 0.0f, 0.8f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 15.0f, 0.0f), 0.0f, 0.0f, 0.001f, glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(12.5f)));
	
	defaultRenderer.addLight(&pointLight)
		.addLight(&pointLight2)
		.addLight(&dirLight)
		.addLight(&spotLight)
		.addLight(&sl2);
	
	// Vertices, normals and texture coordinates for a crate
	float vertices[] = { -0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f,  0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, };
	float normals[] = { 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f };
	float tcoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	// TODO: Fix skybox changes in interface.cpp
	Cubemap* cubemap = new Cubemap("img/skybox/sky/");
	Skybox* skybox = new Skybox(skyboxShader->ID , cubemap);
	
	Model* model = (new Model("models/sea_keep/scene.gltf", phongShader->ID))
		->scaleModel(0.05f, 0.05f, 0.05f)
		->rotateModel(-90.0f, 1.0f, 0.0f, 0.0f);

	//Model model2 = Model("models/tank/scene.gltf", phongShader.ID)
	//	.translateModel(30.0f, 0.0f, 30.0f)
	//	.rotateModel(-90.0f, 1.0f, 0.0f, 0.0f);

	float gridVerts[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	Mesh* grid = new Mesh(gridVerts, sizeof(gridVerts), gridShader->ID);
	grid->setupObject();

	defaultRenderer.addObject(skybox)
		.addObject(model);
		//.addObject(&model2);
		//.addObject(&grid);
	
	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	defaultRenderer.init();

	// Initializes the ImGui UI system
	ImGuiInit(window, &defaultRenderer);
	
	// A simple variable to retrieve the current glGetError() code and decide whether to print it to console
	int glErrorCurrent;
	// A variable that stores the current frame's timestamp, to calculate time between frames
	float currentFrame;

	defaultRenderer.removeObject(model);
	delete model;

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
		
		// Updates matrices to update camera movement
		projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);
		view = camera.getViewMatrix();
		updateUniformBuffer(view, projection);
		
		// Models for phong lighting
		phongShader->use()
			->setVec3("viewPos", camera.position)
			->setVec3("material.ambient", boxTex->ambient)
			->setVec3("material.diffuse", boxTex->diffuse)
			->setVec3("material.specular", boxTex->specular)
			->setFloat("material.shininess", boxTex->shininess);

		// Updates spotlight position and direction based on camera's movement
		spotLight.position = camera.position;
		spotLight.direction = camera.front;

		defaultRenderer.render();
		
		// TODO: Fix grid shader
		gridShader->use();
		//grid.drawObject();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Draws the ImGui interface windows
		ImGuiDrawWindows(camera, boxTex->ambient, boxTex->diffuse, boxTex->specular, boxTex->shininess, skybox);

		// Print error code to console if there is one
		glErrorCurrent = glGetError();
		if (glErrorCurrent != 0) { std::cout << "OpenGL error code: " << glErrorCurrent << std::endl; }

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete phongShader;
	delete skyboxShader;
	delete gridShader;

	delete crate;
	delete boxTex;

	delete cubemap;
	delete skybox;

	//delete model;

	glfwTerminate();
	return 0;
}
