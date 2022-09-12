#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <GLFW/glfw3.h>

#include "headers/camera.hpp"

extern float deltaTime;

void ImGuiInit(GLFWwindow* window);
void ImGuiDrawWindows(Camera &camera, float &strength, glm::vec3 &position);

// See and change camera position, speed etc.
void CameraMenu(Camera& camera);
// Performance info : framerate etc.
void PerformanceMenu();
// Shows the various controls
void KeysMenu();
// Enables modifications of some shader values
void ShaderSettings(float& strength);
void LightSettings(glm::vec3& position);

#endif