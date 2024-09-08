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
	/// Starts the GLFW context and loads GLAD for the OpenGL context
	/// </summary>
	/// <returns>0 if the setup was successful, -1 otherwise</returns>
	int setupGlfwContext();

	/// <summary>
	/// Clears all the keys that are currently being pressed or held
	/// </summary>
	void clearAllKeys();

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
