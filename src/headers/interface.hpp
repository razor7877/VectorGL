#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "headers/camera.hpp"
#include "cubemap.hpp"

extern float deltaTime;

void ImGuiInit(GLFWwindow* window);
void ImGuiDrawWindows(Camera& camera, glm::vec3& position, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess, Cubemap& cubemap);

// See and change camera position, speed etc.
void CameraMenu(Camera& camera);
// Performance info : framerate etc.
void PerformanceMenu();
// Shows the various controls
void KeysMenu();
// Enables modifications of some shader values
void ShaderSettings(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess);
void LightSettings(glm::vec3& position);
void SkyboxSettings(Cubemap& cubemap);

#endif