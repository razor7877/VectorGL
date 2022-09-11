#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "headers/main.hpp"
#include "headers/shader.hpp"
#include "headers/gameObject.hpp"
#include "headers/texture.hpp"
#include "headers/camera.hpp"
#include "headers/interface.hpp"
#include "headers/input.hpp"

const int windowWidth = 1200;
const int windowHeight = 900;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Graphics Programming", NULL, NULL);
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

	Shader shaderProgram = Shader("src/shaders/shader1.vert", "src/shaders/shader1.frag");
	Shader shader2 = Shader("src/shaders/shader2.vert", "src/shaders/shader2.frag");

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  

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
	 0.5f,  0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f, 
	 0.5f,  0.5f,  0.5f,  

	-0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f,  
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

	float v2[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f
	};

	float texCoords[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);

	Texture crate = Texture("img/container.jpg", GL_RGB, false);
	Texture face = Texture("img/awesomeface.png", GL_RGBA, true);

	shaderProgram.use();
	// Set shader uniforms with MVP matrices
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("projection", projection);
	// Set texture samplers
	shaderProgram.setInt("texture1", 0);
	shaderProgram.setInt("texture2", 1);
	
	gameObject cube0 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[0]);
	gameObject cube1 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[1]);
	gameObject cube2 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[2]);
	gameObject cube3 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[3]);
	gameObject cube4 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[4]);
	gameObject cube5 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[5]);
	gameObject cube6 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[6]);
	gameObject cube7 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[7]);
	gameObject cube8 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[8]);
	gameObject cube9 = gameObject(vertices, sizeof(vertices), shaderProgram.ID, cubePositions[9]);

	gameObject containers[] = {
		cube0, cube1, cube2, cube3, cube4, cube5, cube6, cube7, cube8, cube9
	};

	for (int i = 0; i < sizeof(containers) / sizeof(gameObject); i++)
	{
		containers[i].addTexture(crate, tcoords, sizeof(tcoords));
	}

	shader2.use();
	shader2.setMat4("projection", projection);
	shader2.setInt("texture1", 0);

	gameObject square1 = gameObject(v2, sizeof(v2), shader2.ID, glm::vec3(0.0f, 0.0f, 2.0f));
	gameObject square2 = gameObject(v2, sizeof(v2), shader2.ID, glm::vec3(0.0f, 0.0f, -2.0f));
	gameObject square3 = gameObject(v2, sizeof(v2), shader2.ID, glm::vec3(3.0f, 3.0f, 2.0f));

	square1.addTexture(crate, texCoords, sizeof(texCoords));

	gameObject objects[] = {
		square1, square2, square3
	};

	float currentFrame;
	float mixval = 0.5f;
	float fov = camera.zoom;

	// Initializes the ImGui UI system
	ImGuiInit(window);

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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updates matrixes to update camera movement
		projection = glm::perspective(glm::radians(camera.zoom), (float)windowWidth / (float)windowHeight, 0.01f, 100.0f);
		view = camera.getViewMatrix();

		// Updates 2nd shader uniforms
		shader2.use();
		shader2.setMat4("projection", projection);
		shader2.setMat4("view", view);

		objects[0].rotateModel(0.1f, glm::vec3(1.0f, 1.0f, 2.0f));

		for (int i = 0; i < sizeof(objects) / sizeof(gameObject); i++)
		{
			objects[i].drawObject();
		}

		// Updates 1st shader uniforms
		shaderProgram.use();
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		glActiveTexture(GL_TEXTURE1);
		face.bindTexture();

		for (int i = 0; i < sizeof(containers) / sizeof(gameObject); i++)
		{
			containers[i].rotateModel(10.0f * deltaTime * (1+i), 1.0f, 1.0f, 1.0f);
			containers[i].drawObject();
		}

		ImGuiDrawWindows(camera, shaderProgram, mixval);

		// Swap buffers to screen to show the rendered frame
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