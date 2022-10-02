#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
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

glm::vec3 lightPos(1.0f, 1.4f, 4.0f);

// Uniform buffer object (global uniforms)
unsigned int UBO;

void initUniformBuffer()
{
	glGenBuffers(1, &UBO);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2);
}

void updateUniformBuffer(glm::mat4 view, glm::mat4 projection)
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &view[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &projection[0][0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL 3D Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);

	// Set callback functions for window resizing and handling input
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// Check if GLAD loaded successfully
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Enables depth buffering
	glEnable(GL_DEPTH_TEST);

	// Enables face culling for improving performance
	glEnable(GL_CULL_FACE);

	Shader phongShader = Shader("src/shaders/phong.vert", "src/shaders/phong.frag");
	Shader skyboxShader = Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag");
	Shader gridShader = Shader("src/shaders/grid.vert", "src/shaders/grid.frag");

	float skyboxVertices[] = {       
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
	};

	float normals[] = {
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f
	};

	float tcoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);

	Texture crate = Texture("img/container2.png", "texture_diffuse", false);
	Material boxTex = Material(crate);

	// Sets up global uniforms for each shader used
	initUniformBuffer();

	unsigned int UBIphongShader = glGetUniformBlockIndex(phongShader.ID, "Matrices");
	glUniformBlockBinding(phongShader.ID, UBIphongShader, 0);

	unsigned int UBIskyboxShader = glGetUniformBlockIndex(skyboxShader.ID, "Matrices");
	glUniformBlockBinding(skyboxShader.ID, UBIskyboxShader, 0);

	updateUniformBuffer(view, projection);

	glm::vec3 ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	float shininess = 32.0f;

	// Sets up variables for the phong lighting shader
	phongShader.use();
	phongShader.setInt("texture1", 0);
	phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	phongShader.setVec3("lightPos", lightPos);
	phongShader.setVec3("viewPos", camera.position);

	phongShader.setVec3("material.ambient", boxTex.ambient);
	phongShader.setVec3("material.diffuse", boxTex.diffuse);
	phongShader.setVec3("material.specular", boxTex.specular);
	phongShader.setFloat("material.shininess", boxTex.shininess);

	PointLight pointLight = PointLight(glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(15.0f, 5.0f, 5.0f), 1.0f, 0.045f, 0.0075f);
	pointLight.sendToShader(phongShader.ID, 0);
	PointLight light2 = PointLight(glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-5.0f, 5.0f, -5.0f), 1.0f, 0.045f, 0.0075f);
	light2.sendToShader(phongShader.ID, 1);
	phongShader.setInt("nrPointLights", 2);

	DirectionalLight dirLight = DirectionalLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0f), glm::vec3(-0.2f, -1.0f, -0.5f));
	dirLight.sendToShader(phongShader.ID, 0);
	phongShader.setInt("nrDirLights", 1);

	SpotLight spotLight = SpotLight(glm::vec3(0.6f), glm::vec3(0.8f), glm::vec3(1.0f), camera.position, 1.0f, 0.09f, 0.032f, camera.front, glm::cos(glm::radians(12.5f)), 0.0f);
	spotLight.sendToShader(phongShader.ID, 0);
	phongShader.setInt("nrSpotLights", 1);

	// Creates a renderer for drawing objects
	Renderer defaultRenderer = Renderer();

	Mesh instances[1000];
	int i = 0;
	for (int x = 0; x < 100; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			Mesh mesh = Mesh(vertices, sizeof(vertices), phongShader.ID, glm::vec3(x, y, 0.0f));
			mesh.addTexture(crate);
			mesh.addTexCoords(tcoords, sizeof(tcoords));
			mesh.addNormals(normals, sizeof(normals));
			instances[i] = mesh;

			i++;
		}
	}

	for (int i = 0; i < 1000; i++)
	{
		//defaultRenderer.objects.push_back(&instances[i]);
	}

	Mesh light = Mesh(vertices, sizeof(vertices), phongShader.ID, lightPos);
	light.addTexture(crate);
	light.addTexCoords(tcoords, sizeof(tcoords));
	light.scaleMesh(0.25f, 0.25f, 0.25f);;

	Cubemap cubemap = Cubemap("img/skybox/night/");
	Skybox skybox = Skybox(skyboxVertices, sizeof(skyboxVertices), skyboxShader.ID, cubemap);

	Model model = Model("models/sea_keep/scene.gltf", phongShader.ID);
	model.scaleModel(0.05f, 0.05f, 0.05f);
	model.rotateModel(-90.0f, 1.0f, 0.0f, 0.0f);

	Model model2 = Model("models/tank/scene.gltf", phongShader.ID);
	model2.translateModel(30.0f, 0.0f, 30.0f);
	model2.rotateModel(-90.0f, 1.0f, 0.0f, 0.0f);

	defaultRenderer.addObject(&light);
	defaultRenderer.addObject(&skybox);
	defaultRenderer.addObject(&model);
	defaultRenderer.addObject(&model2);

	// After all needed objects have been added, initializes the renderer's data and sets up every object's data
	defaultRenderer.init();

	// Initializes the ImGui UI system
	ImGuiInit(window);

	float currentFrame;

	// Enabled to use the grid shader
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Render loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculates elapsed time since last frame for time-based calculations
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Processes any mouse or keyboard input for camera movement
		processInput(window, deltaTime);

		// Clears the buffers and last frame before rendering the next one
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updates matrices to update camera movement
		projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);
		view = camera.getViewMatrix();

		updateUniformBuffer(view, projection);

		// Models for phong lighting
		phongShader.use();
		phongShader.setVec3("viewPos", camera.position);
		phongShader.setVec3("lightPos", lightPos);
		phongShader.setVec3("material.ambient", boxTex.ambient);
		phongShader.setVec3("material.diffuse", boxTex.diffuse);
		phongShader.setVec3("material.specular", boxTex.specular);
		phongShader.setFloat("material.shininess", boxTex.shininess);

		// Updates movement of the light emitting cube
		light.modelMatrix = glm::mat4(1.0f);
		light.translateMesh(lightPos);
		light.scaleMesh(0.25f, 0.25f, 0.25f);

		// Updates spotlight position and direction based on camera's movement
		spotLight.position = camera.position;
		spotLight.direction = camera.front;
		spotLight.sendToShader(phongShader.ID, 0);

		gridShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		defaultRenderer.render();

		std::cout << glGetError() << std::endl;

		// Draws the ImGui interface windows
		ImGuiDrawWindows(camera, lightPos, boxTex.ambient, boxTex.diffuse, boxTex.specular, boxTex.shininess, skybox);

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// Callback function to handle window size change
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}