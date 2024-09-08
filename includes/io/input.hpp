#ifndef INPUT_HPP
#define INPUT_HPP

#include <map>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "game/gameState.hpp"

// This file contains functions responsible for handling and processing the various user inputs
// such as mouse movement, clicking, scrolling, and keyboard presses

namespace Input
{
	/// <summary>
	/// Represents a single key event registered by GLFW
	/// </summary>
	struct KeyEvent
	{
		int key = 0;
		int scancode = 0;
		int action = GLFW_RELEASE;
		int mods = 0;

		KeyEvent() {}
		KeyEvent(int key, int scancode, int action, int mods) : key(key), scancode(scancode), action(action), mods(mods) {}
	};

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

	// Callback function to handle drag and dropping files
	void dropCallback(GLFWwindow* window, int count, const char** paths);

	/// <summary>
	/// Returns whether a given key is being pressed by the user or not
	/// </summary>
	/// <param name="key">The key that is being checked</param>
	/// <returns>True if the key is being pressed, false otherwise</returns>
	bool isKeyPressed(int key);

	/// <summary>
	/// Returns whether a given key is being held by the user or not
	/// </summary>
	/// <param name="key">The key that is being checked</param>
	/// <returns>True if the key is being held, false otherwise</returns>
	bool isKeyHeld(int key);

	/// <summary>
	/// Returns the current window
	/// </summary>
	/// <returns>A pointer to the GLFW window object</returns>
	GLFWwindow* getWindow();

	/// <summary>
	/// Returns the current window size
	/// </summary>
	/// <returns>A 2D vector containing the size of the window</returns>
	glm::vec2 getWindowSize();
}

#endif
