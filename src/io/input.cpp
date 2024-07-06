#include <iostream>

#include "main.hpp"
#include "io/input.hpp"
#include "components/cameraComponent.hpp"
#include "utilities/resourceLoader.hpp"

GLFWwindow* window;

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

extern Renderer defaultRenderer;

int setupGlfwContext()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VectorGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set callback functions for window resizing and handling input
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetDropCallback(window, drop_callback);

	// Check if GLAD loaded successfully
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 0;
}

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

	cameraComponent->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// If cursor is shown (interacting with UI), don't change FOV
	if (showCursor)
	{
		return;
	}
	cameraComponent->processMouseScroll(static_cast<float>(yoffset));
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
		if (showCursor) // We only allow hiding cursor if viewer is focused
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

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{

	}
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	int i;
	for (i = 0; i < count; i++)
	{
		std::string newPath = std::string(paths[i]);

		int index = 0;
		for (size_t i = 0; i < newPath.length(); ++i)
		{
			if (newPath[i] == '\\')
			{
				newPath[i] = '/';
			}
		}
		std::cout << "Drop callback path: " << newPath << std::endl;
		// TODO : This is not properly disposed of!!
		Entity* newEntity = ResourceLoader::getInstance().loadModelFromFilepath(newPath, LightManager::getInstance().shaderProgramID);
		defaultRenderer.addEntity(newEntity);
		newEntity->start();
	}
}

void processInput(GLFWwindow* window, float &deltaTime)
{
	// If cursor is shown (interacting with UI), don't handle movement
	if (showCursor)
	{
		return;
	}

	// Forward movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraComponent->processKeyboard(CameraMovement::FORWARD, deltaTime);
	}
	// Backward movement
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraComponent->processKeyboard(CameraMovement::BACKWARD, deltaTime);
	}
	// Left movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraComponent->processKeyboard(CameraMovement::LEFT, deltaTime);
	}
	// Right movement
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraComponent->processKeyboard(CameraMovement::RIGHT, deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}