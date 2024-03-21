#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "camera.hpp"
#include "skybox.hpp"
#include "renderer.hpp"

// This file is responsible for handling and displaying the various interfaces that are produced with the ImGUI library

extern float deltaTime;

void ImGuiInit(GLFWwindow* window, Renderer* rendererArg);
void ImGuiDrawWindows(Camera& camera, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess, Skybox* skybox);

// See and change camera position, speed etc.
void CameraMenu(Camera& camera);
// Performance info : framerate etc.
void PerformanceMenu();
// Shows the various controls
void KeysMenu();
// Enables modifications of some shader values
void ShaderSettings(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess);
// Displays the scene's lights and allows real-time modification of their attributes
void LightSettings();
void SkyboxSettings(Skybox* skybox);
void SceneGraphRecurse(std::vector<RenderObject*> children, int& labelIndex);
void SceneGraph();

#endif