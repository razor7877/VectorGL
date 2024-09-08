#include <iostream>

#include "main.hpp"
#include "io/interface.hpp"
#include "io/input.hpp"
#include "utilities/resourceLoader.hpp"
#include "logger.hpp"
#include "game/gameEngine.hpp"
#include "game/gameState.hpp"

namespace Input
{
	struct InputData inputData;

	int setupGlfwContext()
	{
		glfwInit();
		// Set OpenGL 3.3 version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// Set OpenGL core profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// Turn MSAA on, probably not useful now since MSAA is managed and resolved using framebuffers
		glfwWindowHint(GLFW_SAMPLES, 4);

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		inputData.window = glfwCreateWindow(InputData::WINDOW_SIZE.x, InputData::WINDOW_SIZE.y, "VectorGL", NULL, NULL);
		if (inputData.window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(inputData.window);

		// Set callback functions for window resizing and handling input
		glfwSetFramebufferSizeCallback(inputData.window, framebufferSizeCallback);
		glfwSetCursorPosCallback(inputData.window, mouseCallback);
		glfwSetScrollCallback(inputData.window, scrollCallback);
		glfwSetKeyCallback(inputData.window, keyCallback);
		glfwSetDropCallback(inputData.window, dropCallback);

		// Check if GLAD loaded successfully
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

		return 0;
	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (inputData.showCursor)
			return;

		if (inputData.firstMouseCallback)
		{
			inputData.lastCursorPosition.x = (float)xpos;
			inputData.lastCursorPosition.y = (float)ypos;
			inputData.firstMouseCallback = false;
		}

		float xoffset = (float)(xpos - inputData.lastCursorPosition.x);
		float yoffset = (float)(inputData.lastCursorPosition.y - ypos);

		inputData.lastCursorPosition.x = (float)xpos;
		inputData.lastCursorPosition.y = (float)ypos;

		Main::game.getCurrentState()->getScene().currentCamera->processMouseMovement(xoffset, yoffset);
	}

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		// If cursor is shown (interacting with UI), don't change FOV
		if (inputData.showCursor)
		{
			return;
		}
		Main::game.getCurrentState()->getScene().currentCamera->processMouseScroll(static_cast<float>(yoffset));
	}

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Quit program
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		// Toggle to show/hide mouse cursor
		if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS && (Interface::isViewerFocused || !inputData.showCursor))
		{
			inputData.showCursor = !inputData.showCursor;
			if (inputData.showCursor) // We only allow hiding cursor if viewer is focused
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				inputData.firstMouseCallback = true;
			}
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		// Toggle to enable/disable wireframe drawing
		if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
		{
			inputData.wireframeMode = !inputData.wireframeMode;
			if (inputData.wireframeMode)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Toggle to go in or out of fullscreen mode
		if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			inputData.toggleFullscreen = !inputData.toggleFullscreen;
			if (inputData.toggleFullscreen)
			{
				// Saves windowed mode resolution
				inputData.lastWindowSize.x = inputData.windowSize.x;
				inputData.lastWindowSize.y = inputData.windowSize.y;
				// Saves windowed mode coordinates
				glfwGetWindowPos(window, &inputData.lastPosition.x, &inputData.lastPosition.y);

				// Sets window to fullscreen mode and updates viewport
				glfwSetWindowMonitor(window, monitor, 50, 50, mode->width, mode->height, mode->refreshRate);

				// Sets window resolution values to monitor resolution
				inputData.windowSize.x = mode->width;
				inputData.windowSize.y = mode->height;
			}
			else
			{
				// Sets window to windowed mode and updates viewport
				glfwSetWindowMonitor(window, nullptr, inputData.lastPosition.x, inputData.lastPosition.y, inputData.lastWindowSize.x, inputData.lastWindowSize.y, mode->refreshRate);

				// Sets window resolution values to last saved windowed values
				inputData.windowSize.x = inputData.lastWindowSize.x;
				inputData.windowSize.y = inputData.lastWindowSize.y;
			}
		}

		inputData.lambda(&Main::game, inputData.window, 0.0f);
	}

	void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		inputData.windowSize.x = width;
		inputData.windowSize.y = height;
		glViewport(0, 0, width, height);
	}

	void dropCallback(GLFWwindow* window, int count, const char** paths)
	{
		for (int i = 0; i < count; i++)
		{
			std::string newPath = std::string(paths[i]);

			int index = 0;
			for (size_t i = 0; i < newPath.length(); ++i)
			{
				if (newPath[i] == '\\')
					newPath[i] = '/';
			}

			Logger::logInfo("Drag & drop callback path: " + newPath, "input.cpp");

			std::unique_ptr<Entity> newEntity = ResourceLoader::getInstance().loadModelFromFilepath(newPath, LightManager::getInstance().shaderProgram);
			if (newEntity != nullptr)
			{
				newEntity->start();
				Main::game.getCurrentState()->getScene().addEntity(std::move(newEntity));
			}
		}
	}

	void registerKeyLambda(GameState* state, void (*lambda)(GameEngine* gameEngine, GLFWwindow* window, float deltaTime))
	{
		// We set the game state as window user pointer so we can access it later from the lambda
		glfwSetWindowUserPointer(inputData.window, state);
		// Set the new key lambda to be executed whenever the key callback function is called
		inputData.lambda = lambda;
	}

	void unregisterKeyLambda()
	{
		glfwSetWindowUserPointer(inputData.window, nullptr);
		inputData.lambda = nullptr;
	}
}
