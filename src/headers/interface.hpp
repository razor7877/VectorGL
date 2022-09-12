#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <GLFW/glfw3.h>

#include "headers/camera.hpp"

extern float deltaTime;

void ImGuiInit(GLFWwindow* window);
void ImGuiDrawWindows(Camera &camera);

void CameraMenu(Camera& camera);
void PerformanceMenu();

#endif