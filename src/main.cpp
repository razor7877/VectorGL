#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "io/input.hpp"
#include "io/interface.hpp"
#include "logger.hpp"
#include "renderer.hpp"
#include "game/gameEngine.hpp"
#include "game/mainGameState.hpp"

extern GLFWwindow* window;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Creates a renderer for storing & drawing objects
Renderer defaultRenderer = Renderer();
CameraComponent* cameraComponent = nullptr;

int main()
{
	if (setupGlfwContext() != 0)
		return -1;
	
	// Initializes the ImGui UI system
	ImGuiInit(window, &defaultRenderer);

	// Start the game state
	GameEngine game;
	MainGameState mainState = MainGameState(defaultRenderer);
	game.init();
	game.changeState(&mainState);
	cameraComponent = mainState.camera;
	
	// A simple variable to retrieve the current glGetError() code and decide whether to print it to console
	int glErrorCurrent;
	// A variable that stores the current frame's timestamp, to calculate time between frames
	float currentFrame;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Clears the buffers and last frame before rendering the next one
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Calculates elapsed time since last frame for time-based calculations
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Processes any mouse or keyboard input for camera movement
		processInput(window, deltaTime);

		game.handleEvents();
		game.update(deltaTime);
		game.draw();
		
		// Draws the ImGui interface windows
		ImGuiDrawWindows();

		// Print error code to console if there is one
		glErrorCurrent = glGetError();
		if (glErrorCurrent != 0) { Logger::logError(std::string("OpenGL error code: ") + std::to_string(glErrorCurrent), "main.cpp"); }

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	game.cleanup();

	glfwTerminate();
	return 0;
}
