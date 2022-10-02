#include <iostream>

#include "main.hpp"
#include "io/input.hpp"
#include "camera.hpp"

float lastX = (float)windowWidth / 2;
float lastY = (float)windowHeight / 2;

bool showCursor = false;
bool wireframeMode = false;
bool firstMouse = true;
bool toggleFullscreen = false;

// When toggling fullscreen on, save the resolution of window
// If fullscreen is then toggled off, set window size to these values
int lastWidth = WINDOW_WIDTH;
int lastHeight = WINDOW_HEIGHT;
// Used to save the last coordinates of the windowed mode window
int lastXPos = 50;
int lastYPos = 50;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (showCursor)
	{
		return;
	}

	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)(xpos - lastX);
	float yoffset = (float)(lastY - ypos);

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Quit program
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Toggle to show/hide mouse cursor
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		showCursor = !showCursor;
		if (showCursor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	// Toggle to enable/disable wireframe drawing
	if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
	{
		wireframeMode = !wireframeMode;
		if (wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	// Toggle to go in or out of fullscreen mode
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		toggleFullscreen = !toggleFullscreen;
		if (toggleFullscreen)
		{
			// Saves windowed mode resolution
			lastWidth = windowWidth;
			lastHeight = windowHeight;
			// Saves windowed mode coordinates
			glfwGetWindowPos(window, &lastXPos, &lastYPos);

			// Sets window to fullscreen mode and updates viewport
			glfwSetWindowMonitor(window, monitor, 50, 50, mode->width, mode->height, mode->refreshRate);
			glViewport(0, 0, mode->width, mode->height);

			// Sets window resolution values to monitor resolution
			windowWidth = mode->width;
			windowHeight = mode->height;
		}
		else
		{
			// Sets window to windowed mode and updates viewport
			glfwSetWindowMonitor(window, nullptr, lastXPos, lastYPos, lastWidth, lastHeight, mode->refreshRate);
			glViewport(0, 0, lastWidth, lastHeight);

			std::cout << lastXPos << " " << lastYPos << std::endl;

			// Sets window resolution values to last saved windowed values
			windowWidth = lastWidth;
			windowHeight = lastHeight;
		}
	}
}

void processInput(GLFWwindow* window, float &deltaTime)
{
	// If cursor is shown, don't handle movement
	if (showCursor)
	{
		return;
	}

	// Forward movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
	}
	// Backward movement
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
	}
	// Left movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(CameraMovement::LEFT, deltaTime);
	}
	// Right movement
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
	}
}