#ifndef INPUT_HPP
#define INPUT_HPP

#include <glfw/glfw3.h>
#include <glm/glm.hpp>

// This file contains functions responsible for handling and processing the various user inputs
// such as mouse movement, clicking, scrolling, and keyboard presses

namespace Input
{
	/// <summary>
/// A struct used to store data relevant to the input.cpp file
/// Stores information on the window size, position, fullscreen status, cursor status etc.
/// </summary>
	struct InputData
	{
		/// <summary>
		/// The default window size on startup
		/// </summary>
		static constexpr glm::ivec2 WINDOW_SIZE = glm::ivec2(1920, 1080);

		/// <summary>
		/// A pointer to the GLFW window object
		/// </summary>
		GLFWwindow* window = nullptr;

		/// <summary>
		/// Whether the cursor should be shown
		/// </summary>
		bool showCursor = true;

		/// <summary>
		/// Whether the OpenGL wireframe drawing mode is enabled
		/// </summary>
		bool wireframeMode = false;

		/// <summary>
		/// Whether a first mouse callback event was recorded
		/// </summary>
		bool firstMouseCallback = true;

		/// <summary>
		/// Whether the fullscreen mode is enabled or not
		/// </summary>
		bool toggleFullscreen = false;

		/// <summary>
		/// The last position of the mouse cursor
		/// </summary>
		glm::ivec2 lastCursorPosition = glm::ivec2((float)InputData::WINDOW_SIZE.x / 2.0f, (float)InputData::WINDOW_SIZE.y / 2.0f);

		/// <summary>
		/// The current size of the window
		/// </summary>
		glm::ivec2 windowSize = glm::ivec2(InputData::WINDOW_SIZE.x, InputData::WINDOW_SIZE.y);

		/// <summary>
		/// The size of the windowed window before switching to fullscreen mode
		/// </summary>
		glm::ivec2 lastWindowSize = glm::ivec2(InputData::WINDOW_SIZE.x, InputData::WINDOW_SIZE.y);

		/// <summary>
		/// The position of the windowed window before switching to fullscreen mode
		/// </summary>
		glm::ivec2 lastPosition = glm::ivec2(50);
	};

	extern struct InputData inputData;

	/// <summary>
	/// Starts the GLFW context and loads GLAD for the OpenGL context
	/// </summary>
	/// <returns>0 if the setup was successful, -1 otherwise</returns>
	int setupGlfwContext();

	// Callback function to register mouse input
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	// Callback function to register scrolling
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	// Used to register only first key press of a given key
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Callback function to handle window size change
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	// Callbakc function to handle drag and dropping files
	void dropCallback(GLFWwindow* window, int count, const char** paths);

	/// <summary>
	/// A function that can receive any lambda function to process received inputs as needed depending
	/// on game state etc.
	/// </summary>
	/// <typeparam name="T">A lambda that receives a GLFWwindow* and a float as parameters</typeparam>
	/// <param name="lambda">The lambda to use for processing the inputs</param>
	/// <param name="window">A pointer to the GLFWwindow pointer object for querying inputs</param>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	template <typename T>
	void processInput(T&& lambda, GLFWwindow* window, float deltaTime)
	{
		lambda(window, deltaTime);
	}
}

#endif