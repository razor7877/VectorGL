#ifndef MAIN_HPP
#define MAIN_HPP

#include <GLFW/glfw3.h>

#include "headers/camera.hpp"

extern const int windowWidth;
extern const int windowHeight;

extern Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

#endif