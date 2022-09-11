#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <GLFW/glfw3.h>

#include "headers/camera.hpp"

void ImGuiInit(GLFWwindow* window);
void ImGuiDrawWindows(Camera &camera, Shader shaderProgram, float &mixval);

#endif