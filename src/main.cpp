#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "io/input.hpp"
#include "io/interface.hpp"
#include "logger.hpp"
#include "game/gameEngine.hpp"
#include "game/startMenuState.hpp"
#include "main.hpp"

using namespace Main;

namespace Main
{
	/// <summary>
	/// A reference to the game engine, which manages nearly all the state of the engine
	/// </summary>
	GameEngine game = GameEngine();

	/// <summary>
	/// Stores the current elapsed time since the last frame
	/// </summary>
	float deltaTime = 0.0f;

	/// <summary>
	/// The timestamp of the previous frame
	/// </summary>
	float lastFrame = 0.0f;

	int frameCounter = 0;
}

int main()
{
	if (Input::setupGlfwContext() != 0)
		return -1;

	// Initializes the ImGui UI system
	GLFWwindow* window = Input::getWindow();
	Interface::ImGuiInit(window);

	// Start the game state
	std::unique_ptr<StartMenuState> mainState = std::make_unique<StartMenuState>(game.renderer);
	game.init();
	game.changeState(std::move(mainState));

	// A simple variable to retrieve the current glGetError() code and decide whether to print it to console
	int glErrorCurrent;
	// A variable that stores the current frame's timestamp, to calculate time between frames
	float currentFrame;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculates elapsed time since last frame for time-based calculations
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		game.handleEvents(deltaTime);
		game.update(deltaTime);
		game.draw(deltaTime);

		// Draws the ImGui interface windows
		Interface::ImGuiDrawWindows();

		// Print error code to console if there is one
		glErrorCurrent = glGetError();
		if (glErrorCurrent != 0) { Logger::logError(std::string("OpenGL error code: ") + std::to_string(glErrorCurrent), "main.cpp"); }

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		frameCounter++;
	}

	game.cleanup();

	glfwTerminate();
	return 0;
}
