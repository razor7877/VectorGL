#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "headers/main.hpp"
#include "headers/shader.hpp"
#include "headers/model.hpp"
#include "headers/Mesh.hpp"
#include "headers/skybox.hpp"
#include "headers/texture.hpp"
#include "headers/camera.hpp"
#include "headers/interface.hpp"
#include "headers/input.hpp"
#include "headers/cubemap.hpp"
#include "headers/renderer.hpp"

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

	float trivertices[] = {
		// first triangle
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f,  0.5f, 0.0f,  // top left 
		// second triangle
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
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
	phongShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	phongShader.setVec3("lightPos", lightPos);
	phongShader.setVec3("viewPos", camera.position);

	phongShader.setVec3("material.ambient", boxTex.ambient);
	phongShader.setVec3("material.diffuse", boxTex.diffuse);
	phongShader.setVec3("material.specular", boxTex.specular);
	phongShader.setFloat("material.shininess", boxTex.shininess);

	// Creates a renderer for drawing objects
	Renderer defaultRenderer = Renderer();
	
	Mesh cube = Mesh(vertices, sizeof(vertices), phongShader.ID, glm::vec3(0.0f, 0.0f, 2.0f));
	//cube.addMaterial(boxTex, tcoords, sizeof(tcoords));
	cube.addNormals(normals, sizeof(normals));

	Mesh instances[1000];
	int i = 0;
	for (int x = 0; x < 100; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			Mesh mesh = Mesh(vertices, sizeof(vertices), phongShader.ID, glm::vec3(x, y, 0.0f));
			//mesh.addMaterial(boxTex, tcoords, sizeof(tcoords));
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
	//light.addMaterial(boxTex, tcoords, sizeof(tcoords));
	light.addTexture(crate);
	light.addTexCoords(tcoords, sizeof(tcoords));
	light.scaleModel(0.25f, 0.25f, 0.25f);;

	Cubemap cubemap = Cubemap("img/skybox/night/");
	Skybox skybox = Skybox(skyboxVertices, sizeof(skyboxVertices), skyboxShader.ID, cubemap);

	float rectV[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int rectI[] = {
		0, 1, 3,
		1, 2, 3
	};

	Mesh rectangle = Mesh(rectV, sizeof(rectV), phongShader.ID, glm::vec3(-5.0f, 0.0f, 0.0f));
	rectangle.addIndices(rectI, sizeof(rectI));

	Model model = Model("models/tank/scene.gltf", phongShader.ID);

	//defaultRenderer.objects.push_back(&rectangle);
	//defaultRenderer.objects.push_back(&light);
	//defaultRenderer.objects.push_back(&cube);
	defaultRenderer.objects.push_back(&skybox);
	
	defaultRenderer.objects.push_back(&model);

	// After all needed objects have been added, initializes the renderer's data and sets up every object's data
	defaultRenderer.init();

	// Initializes the ImGui UI system
	ImGuiInit(window);

	float strength = 0.0f;
	float currentFrame;

	glDisable(GL_CULL_FACE);

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

		// Updates matrixes to update camera movement
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

		// Update movement of the light emitting cube
		light.modelMatrix = glm::mat4(1.0f);
		light.translateModel(lightPos);
		light.scaleModel(0.25f, 0.25f, 0.25f);

		defaultRenderer.render();
		std::cout << glGetError() << std::endl;

		gridShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draws the ImGui interface windows
		ImGuiDrawWindows(camera, lightPos, boxTex.ambient, boxTex.diffuse, boxTex.specular, boxTex.shininess, cubemap);

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